;;; GNU Guix development package.
;;;
;;; To build and install, close this repository, and from the project root run
;;; the following:
;;;
;;;     guix package --install-from-file=jbuild.scm
;;;
;;; To bootstrap a development environment container, the following command is
;;; helpful:
;;;
;;;     guix shell --container --development --file=jbuild.scm
;;;

(define-module (jsoftware)
  #:use-module ((ice-9 popen) #:select (open-pipe*
                                        close-pipe))
  #:use-module ((ice-9 rdelim) #:select (read-line))
  #:use-module ((ice-9 match) #:select (match match-lambda))
  #:use-module ((srfi srfi-1) #:select (any every))
  #:use-module ((srfi srfi-26) #:select (cut))
  #:use-module ((guix gexp) #:select (gexp
                                      local-file
                                      program-file))
  #:use-module ((guix records) #:select (define-record-type*))
  #:use-module ((guix licenses) #:prefix license: #:select (gpl3+))
  #:use-module ((guix utils) #:select (target-arm? target-64bit?))
  #:use-module ((guix build-system gnu) #:select (gnu-build-system
                                                  %gnu-build-system-modules))
  #:use-module ((guix build-system trivial) #:select (trivial-build-system))
  #:use-module ((guix build utils) #:select (substitute* with-directory-excursion))
  #:use-module ((guix packages) #:select (base32
                                          origin
                                          package
                                          package/inherit
                                          package-properties
                                          package-version))
  #:use-module ((gnu packages) #:select (search-patches))
  #:use-module ((gnu packages libedit) #:select (libedit))
  #:use-module ((gnu packages llvm) #:select (clang-toolchain
                                              libomp))
  #:use-module ((gnu packages maths) #:select (sleef)))


(define %source-dir (dirname (current-filename)))

(define (git-version)
  "Return a version string suitable for development builds."
  (let* ((pipe (with-directory-excursion %source-dir
                 (open-pipe* OPEN_READ "git" "describe" "--always" "--tags")))
         (version (read-line pipe)))
    (close-pipe pipe)
    version))

(define (git-user)
  "Return a user info string scraped from Git."
  (let* ((name-pipe (with-directory-excursion %source-dir
                      (open-pipe* OPEN_READ "git" "config" "user.name")))
         (email-pipe (with-directory-excursion %source-dir
                       (open-pipe* OPEN_READ "git" "config" "user.email")))
         (name (read-line name-pipe))
         (email (read-line email-pipe))
         (status (every identity (map close-pipe `(,name-pipe ,email-pipe)))))
    (format #f "~a <~a>" name email)))

;; Predicate intended for SELECT? argument of local-file procedure. Returns
;; true if and only if file is tracked by git.
(define git-file?
  (let* ((pipe (with-directory-excursion %source-dir
                 (open-pipe* OPEN_READ "git" "ls-files")))
         (files (let loop ((lines '()))
                  (match (read-line pipe)
                    ((? eof-object?) (reverse lines))
                    (line (loop (cons line lines))))))
         (status (close-pipe pipe)))
    (lambda (file stat)
      (match (stat:type stat)
        ('directory #t)
        ((or 'regular 'symlink) (any (cut string-suffix? <> file) files))
        (_ #f)))))


(define (ijconsole)
  "Generate a G-exp script that detects AVX/AVX2 support at runtime and
  executes jconsole with the appropriate libj.so and profile.ijs."
  (program-file "ijconsole"
    #~(begin
        (use-modules ((ice-9 rdelim) #:select (read-line))
                     ((ice-9 regex) #:select (regexp-match? string-match)))

        ;; Assume that this script will be installed under bin/.
        (define %basedir (dirname (dirname (current-filename))))

        (define (cpu-feature-line? string)
          (string-prefix? "flags" string))

        (define (contains-word? word string)
          (regexp-match?
            (string-match (string-join `("\\<" ,word "\\>") "")
                          string)))

        (define (has-cpu-feature? feature)
          (with-input-from-file "/proc/cpuinfo"
             (lambda ()
               (catch 'found
                 (lambda ()
                   (let loop ((line (read-line)))
                     (cond ((eof-object? line) #f)
                           ((and (cpu-feature-line? line)
                                 (contains-word? feature line))
                            (throw 'found))
                           (else (loop (read-line))))))
                 (const #t)))))

        (let* ((jconsole (string-append %basedir "/libexec/j/jconsole"))
               (libj (format #f "~a/lib/j/libj-~a.so" %basedir
                             (cond ((has-cpu-feature? "avx2") "avx2")
                                   ((has-cpu-feature? "avx") "avx")
                                   (else ""))))
               (jprofile (string-append %basedir "/etc/j/profile.ijs")))
          (apply execl jconsole "ijconsole" "-lib" libj "-jprofile" jprofile
                 (cdr (command-line)))))))


(define* (j-package #:key (builder (git-user))
                          (version (git-version))
                          (type 'devel)
                          (extra-inputs '())
                          (extra-envars '()))
 (package
   (name "j")
   (version version)
   (source (local-file %source-dir #:recursive? #t #:select? git-file?))
   (build-system gnu-build-system)
   (native-inputs `(("clang-toolchain" ,clang-toolchain)))
   (inputs (cons* `("libedit" ,libedit)
                  `("libomp" ,libomp)
                  `("ijconsole" ,(ijconsole))
                  extra-inputs))
   (arguments
    `(#:tests? #f
      #:modules (((ice-9 ftw) #:select (scandir))
                 ((ice-9 popen) #:select (open-pipe* close-pipe))
                 ((ice-9 regex) #:select (match:substring string-match))
                 ((ice-9 threads) #:select (parallel par-for-each))
                 ((srfi srfi-26) #:select (cut))
                 ((srfi srfi-1) #:select (fold))
                 ,@%gnu-build-system-modules)
      #:phases
      ;; Upstream's build system consists of ad-hoc scripts that build
      ;; build up (very complicated) environment variables to pass to make.
      ;; The basic build process looks like this:
      ;;
      ;;   1) Copy jsrc/jversion-x.h to jsrc/jversion.h and edit values;
      ;;   2) Set jplatform and j64x environment variables;
      ;;   3) Run make2/build_jconsole.sh and make2/build_libj.sh;
      ;;
      ;; However, upstream expects users to run J directly from the source
      ;; directory; they do not supply a make `install' target.  Thus it
      ;; takes some massaging to install files in FHS-style directories.
      (modify-phases %standard-phases
        ;; In particular, we have to set up
        ;;
        ;;   1) jsrc/jversion.h as in a typical build;
        ;;   2) jlibrary/bin/profilex.ijs to point to writable directories;
        ;;   3) make2/build_*.sh to respect standard build conventions;
        ;;   4) jsrc/jconsole.c to fix libedit dlopen; and
        ;;   5) Hard coded references to addons directory.
        (replace 'configure
          (lambda* (#:key target inputs outputs #:allow-other-keys)
            (let* ((clang-toolchain (assoc-ref inputs "clang-toolchain"))
                   (clang (string-append clang-toolchain "/bin/clang"))
                   (libedit (assoc-ref inputs "libedit"))
                   (out (assoc-ref outputs "out")))
              ;; Set up build constants
              (copy-file "jsrc/jversion-x.h" "jsrc/jversion.h")
              (substitute* "jsrc/jversion.h"
                (("^#define jversion.*$")
                 (format #f "#define jversion ~s\n" ,version))
                (("^#define jtype.*$")
                 (format #f "#define jtype ~s\n" (symbol->string ',type)))
                (("^#define jbuilder.*$")
                 (format #f "#define jbuilder ~s\n" ,builder)))
              ;; Create profilex.ijs overrides to point to the correct
              ;; store items.  Note that we set ~install and ~addons
              ;; directories to reside under ~user to allow installing
              ;; and loading addons.  TODO: Guix-ify J addons as well.
              (call-with-output-file "jlibrary/bin/profilex.ijs"
                (lambda (port)
                  (display
                    (string-join
                      (list
                        "share=. '/share/j',~ ({.~ _2 { I.@:=&'/') BINPATH"
                        "system=. share,'/system'"
                        "tools=. share,'/tools'"
                        ;; Upstream defaults to spamming $HOME with
                        ;; unhidden userdata directories.  Set this to be
                        ;; $HOME/.j/<jtype>/<jversion> instead
                        "'jtype jversion'=. (3&{,{.) <;._2 ,&'/' 9!:14''"
                        "jversion=. ({.~ i.&'-') jversion"
                        "jsuffix=. >@{&('';'-beta') jtype -: 'beta'"
                        "user=. home,'/.j/',jversion,jsuffix"
                        "addons=. user,'/addons'"
                        "break=. user,'/break'"
                        "config=. user,'/config'"
                        "install=. user,'/install'"
                        "snap=. user,'/snap'"
                        "temp=. user,'/temp'"
                        "\n")
                      "\n")
                    port)))
              ;; Munge the build scripts into reason:
              ;; 1. Short-circuit the fragile compiler detection;
              ;; 2. Make sure to include our CFLAGS and LFLAGS; and
              ;; 3. Propagate script errors to top level.
              (for-each
               (lambda (file)
                 (with-directory-excursion "make2"
                   (substitute* file
                     ;; The `compiler' variable doesn't point to the actual
                     ;; compiler.  It is just a switch to tell the build
                     ;; scripts whether to use gcc- or clang-specific
                     ;; flags.
                     (("^compiler=.*$") "compiler=clang\n")
                     (("^LDFLAGS=\"" def) (string-append def "$LDFLAGS "))
                     (("^(common=\")(\\$USETHREAD.*)$" _ def rest)
                      (string-append def "$CFLAGS " rest))
                     (("^#!.*" shebang)
                      (string-append shebang "set -o errexit\n")))))
                 '("build_jconsole.sh" "build_libj.sh"))
              ;; The jconsole manually loads libedit with dlopen.  The path
              ;; must be absolute to correctly point to our input.
              (substitute* "jsrc/jconsole.c"
                (("libedit\\.so\\.[0-9]" so-file)
                 (format #f "~a/lib/~a" libedit so-file)))
              ;; The ~addons/dev directory supplies tentative J-script
              ;; definitions of new J engine functionality.  Since we point
              ;; ~addons under the ~user directory, we move it under
              ;; ~system instead, which sits as-is in the output.
              (with-directory-excursion "jsrc"
                (for-each
                  (lambda (file)
                    (substitute* file (("~addons/dev") "~system/dev")))
                  (scandir "."
                    (lambda (f) (eq? (stat:type (stat f)) 'regular)))))
              ;; Implementation of 9!:14 records build time which breaks
              ;; build reproducibility.  Note that upstream code depends on
              ;; the exact format of these strings, so we need to mimic the
              ;; standard.
              (substitute* "jsrc/j.c"
                (("__DATE__") "\"Jan 01 1970\"")
                (("__TIME__") "\"00:00:00\""))
              ;; Upstream recommends using clang, with GCC support being
              ;; second-class, often resulting in build failures.
              (setenv "CC" clang))))
        ;; The build output depends primarily on the values of the
        ;; `jplatform' and `j64x' environment variables.  If the target is
        ;; ARM, then `jplatform' is "raspberry", otherwise it is `linux'.
        ;; In addition to 32- and 64- bit versions, `j64x' controlls
        ;; whether AVX or AVX2 variants of libj are built.
        ;;
        ;; However, build targets are not fine-grained enough to
        ;; distinguish between CPU features.  Thus we build and install all
        ;; variants of libj, expecting jconsole to be called with a wrapper
        ;; script that detects AVX features and loads the appropriate libj
        ;; at runtime.
        (replace 'build
          (lambda _
            (setenv "USE_OPENMP" "1")
            (setenv "USE_THREAD" "1")
            (for-each (lambda (var-val) (apply setenv var-val))
                      (quote ,extra-envars))
            ;; The build scripts assume that PWD is make2.
            (with-directory-excursion "make2"
              (let* ((platform ,(if (target-arm?) "raspberry" "linux"))
                     (jplat (string-append "jplatform=" platform))
                     (target-bit ,(if (target-64bit?) "64" "32"))
                     (jbit (string-append "j64x=" "j" target-bit))
                     (jbit-avx (string-append jbit "avx"))
                     (jbit-avx2 (string-append jbit "avx2")))
                (parallel
                  ;; Since jconsole doesn't depend on AVX features, we just
                  ;; build it once.
                  (invoke "env" jplat jbit "./build_jconsole.sh")
                  (invoke "env" jplat jbit "./build_libj.sh")
                  (if ,(target-64bit?)
                    (parallel
                      (invoke "env" jplat jbit-avx "./build_libj.sh")
                      (invoke "env" jplat jbit-avx2
                              "./build_libj.sh"))))))))
        ;; The test suite is expected to be run as follows for each variant
        ;; of libj that we build:
        ;;
        ;;     $ echo 'RUN ddall' | jconsole test/tsu.ijs
        ;;
        ;; This requires a working jconsole with accessible jlibrary files.
        ;; We simply place these all under test/bin.
        (replace 'check
          (lambda* (#:key tests? #:allow-other-keys)
            (when tests?
              (let ((jplatform ,(if (target-arm?) "raspberry" "linux")))
                (mkdir-p "test/bin")
                (for-each
                  (lambda (dir)
                    (let ((source (string-append "jlibrary/" dir))
                          (dest (string-append "test/bin/" dir)))
                    (begin
                      (mkdir-p dest)
                      (copy-recursively source dest))))
                  '("system" "tools" "addons"))
                ;; The jlibrary/dev directory only sometimes exists, but
                ;; needs to be copied into the ~system directory when it
                ;; does.
                (for-each
                  (lambda (dev-dir)
                    (if (access? dev-dir R_OK)
                      (copy-recursively dev-dir "test/bin/system/dev")))
                  '("jlibrary/dev" "jlibrary/addons/dev"))
                (par-for-each
                  (lambda (dir)
                    (let* ((jbin (string-append "bin/" jplatform))
                           (jbit ,(if (target-64bit?) "j64" "j32"))
                           (jconsole (string-append jbin "/" jbit
                                                    "/jconsole"))
                           (source (string-append jbin "/" dir))
                           (dest (string-append "test/bin/" dir)))
                      (begin
                        (mkdir-p dest)
                        (copy-recursively source dest)
                        (install-file "jlibrary/bin/profile.ijs" dest)
                        (install-file jconsole dest)
                        (let* ((jc (string-append dest "/jconsole"))
                               (tests "test/tsu.ijs")
                               (port (open-pipe* OPEN_WRITE jc tests)))
                          (display "RUN ddall\n" port)
                          (when (not (zero? (status:exit-val
                                              (close-pipe port))))
                            (error "Some J build tests failed."))))))
                  (scandir (string-append "bin/" jplatform)
                           (negate (cut member <> '("." "..")))))
                #t))))
        ;; Now that everything is built, installation is fairly
        ;; straightforward, following FHS conventions.  The only quirk is
        ;; that we install jconsole under /libexec to make room for the
        ;; wrapper replacement under /bin.
        (replace 'install
          (lambda* (#:key outputs inputs #:allow-other-keys)
            (let* ((jplat ,(if (target-arm?) "raspberry" "linux"))
                   (jbit ,(if (target-64bit?) "j64" "j32"))
                   (interp (string-join `("bin" ,jplat ,jbit "jconsole") "/"))
                   (ijconsole (assoc-ref inputs "ijconsole"))
                   (vname (match:substring (string-match "[0-9]+" ,version)))
                   (out (assoc-ref outputs "out"))
                   (bin (string-append out "/bin"))
                   (etc (string-append out "/etc/j"))
                   (lib (string-append out "/lib/j"))
                   (libexec (string-append out "/libexec/j"))
                   (share (string-append out "/share/j"))
                   (system (string-append share "/system"))
                   (dev (string-append system "/dev")))
              (mkdir-p bin)
              (copy-file ijconsole (string-append bin "/ijconsole-" vname))
              (mkdir-p lib)
              (for-each
                (lambda (jarch)
                  (let* ((jbin (string-join `("bin" ,jplat ,jarch) "/"))
                         (javx-match (string-match "avx.*" jarch))
                         (javx (if (not javx-match) ""
                                 (match:substring javx-match)))
                         (sep (if javx-match "-" ""))
                         (source (string-append jbin "/libj.so"))
                         (dest (format #f "~a/libj~a~a.so" lib sep javx)))
                    (copy-file source dest)))
                (scandir (string-append "bin/" jplat)
                         (negate (cut member <> '("." "..")))))
              (install-file interp libexec)
              (copy-recursively "jlibrary/system" system)
              (for-each
                (lambda (source-dev)
                  (if (access? source-dev R_OK)
                    (copy-recursively source-dev dev)))
                '("jlibrary/dev" "jlibrary/addons/dev"))
              (install-file "jlibrary/bin/profile.ijs" etc)
              (install-file "jlibrary/bin/profilex.ijs" etc)))))))
   (home-page "https://www.jsoftware.com/")
   (synopsis "Ascii-only, array programming language in the APL family")
   (description
 "J is a high-level, general-purpose programming language that is
particularly suited to the mathematical, statistical, and logical analysis of
data. It is a powerful tool for developing algorithms and exploring problems
that are not already well understood.")
   (license license:gpl3+)))

(j-package #:extra-inputs `(("sleef" ,sleef))
           #:extra-envars `(("USE_SLEEF_SRC" "0")
                            ("LDFLAGS" "-lsleef")))
