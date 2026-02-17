#!/usr/bin/env sh
dir=${0%/*}; dir=${dir:-.}
exec guix time-machine --channels=$dir/channels.scm -- repl "$0"
!#
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
  #:use-module ((ice-9 regex) #:select (string-match))
  #:use-module ((ice-9 match) #:select (match match-lambda))
  #:use-module ((srfi srfi-1) #:select (any concatenate every zip))
  #:use-module ((srfi srfi-26) #:select (cut))
  #:use-module ((guix base32) #:select (bytevector->base32-string))
  #:use-module ((guix colors) #:select (color
                                        colorize-string))
  #:use-module ((guix derivations) #:select (built-derivations
                                             derivation-outputs
                                             derivation-output-path))
  #:use-module ((guix gexp) #:select (gexp
                                      file-union
                                      local-file
                                      plain-file
                                      program-file
                                      with-imported-modules))
  #:use-module ((guix hash) #:select (file-hash*))
  #:use-module ((guix records) #:select (define-record-type*))
  #:use-module ((guix licenses) #:prefix license: #:select (gpl3+))
  #:use-module ((guix monads) #:select (mbegin
                                        mapm
                                        mlet*
                                        return))
  #:use-module ((guix utils) #:select (target-arm? target-64bit?))
  #:use-module ((guix build-system gnu) #:select (gnu-build-system
                                                  %gnu-build-system-modules))
  #:use-module ((guix build-system trivial) #:select (trivial-build-system))
  #:use-module ((guix build utils) #:select (substitute* with-directory-excursion))
  #:use-module ((guix packages) #:select (base32
                                          origin
                                          package
                                          package->derivation
                                          package/inherit
                                          package-grafts
                                          package-properties
                                          package-version))
  #:use-module ((guix store) #:select (%store-monad
                                       open-connection
                                       run-with-store
                                       store-lift))
  #:use-module ((gnu packages) #:select (search-patches))
  #:use-module ((gnu packages libedit) #:select (libedit))
  #:use-module ((gnu packages llvm) #:select (clang-toolchain
                                              clang-toolchain-12
                                              libomp))
  #:use-module ((gnu packages maths) #:select (sleef)))


(define %source-dir
  (let ((pipe (open-pipe* OPEN_READ "git" "rev-parse" "--show-toplevel")))
    (read-line pipe)))

(define (git-version)
  "Return a version string suitable for development builds."
  (with-directory-excursion %source-dir
    (let* ((pipe (open-pipe* OPEN_READ "git" "describe" "--always"
                                                        "--tags"
                                                        "--abbrev=0"))
           (version (string-append (read-line pipe) "+git")))
      (close-pipe pipe)
      version)))

(define (git-user)
  "Return a user info string scraped from Git."
  (with-directory-excursion %source-dir
    (let* ((name-pipe (open-pipe* OPEN_READ "git" "config" "user.name"))
           (email-pipe (open-pipe* OPEN_READ "git" "config" "user.email"))
           (name (read-line name-pipe))
           (email (read-line email-pipe))
           (status (every identity (map close-pipe `(,name-pipe ,email-pipe)))))
      (format #f "~a <~a>" name email))))

;; Predicate intended for SELECT? argument of local-file procedure. Returns
;; true if and only if file is tracked by git.
(define git-file?
  (with-directory-excursion %source-dir
    (let* ((pipe (open-pipe* OPEN_READ "git" "ls-files"))
           (files (let loop ((lines '()))
                    (match (read-line pipe)
                      ((? eof-object?) (reverse lines))
                      ((? (lambda (file)   ; skip this file
                            (string-match (current-filename)
                                          (canonicalize-path file))))
                       (loop lines))
                      (line (loop (cons line lines))))))
           (status (close-pipe pipe)))
      (lambda (file stat)
        (match (stat:type stat)
          ('directory #t)
          ((or 'regular 'symlink) (any (cut string-suffix? <> file) files))
          (_ #f))))))


;; G-exp script that detects AVX/AVX2 support at runtime and executes jconsole
;; with the appropriate libj.so and profile.ijs."
(define ijconsole
  (with-imported-modules '((guix cpu)
                           (guix memoization)
                           (guix profiling)
                           (guix sets)
                           (srfi srfi-26))
    (program-file "ijconsole"
      #~(begin
          (use-modules ((guix cpu)     #:select (cpu-flags current-cpu))
                       ((guix sets)    #:select (set-contains?))
                       ((srfi srfi-26) #:select (cute)))

          ;; Assume that this script will be installed under bin/.
          (define %basedir (dirname (dirname (current-filename))))

          (let* ((jconsole (string-append %basedir "/libexec/j/jconsole"))
                 (cpu-has-flag?
                   (cute set-contains? (cpu-flags (current-cpu)) <>))
                 (libj (format #f "~a/lib/j/libj~a.so" %basedir
                               (cond ((cpu-has-flag? "avx2") "-avx2")
                                     ((cpu-has-flag? "avx") "-avx")
                                     (else ""))))
                 (jprofile (string-append %basedir "/etc/j/profile.ijs")))
            (apply execl jconsole "ijconsole" "-lib" libj "-jprofile" jprofile
                   (cdr (command-line))))))))


(define jsoftware-auxiliary-files
  (file-union "jsoftware-auxiliary-files"
    `(("bin/ijconsole" ,ijconsole)
      ;; profilex.ijs overrides ~install and ~addons directories to reside
      ;; under the user-writable ~user.  This allows local-install of addons
      ;; via pacman.  TODO: Guix-ify J addons as well.
      ("etc/j/profilex.ijs"
       ,(plain-file "profilex.ijs"
          (string-join
            '("'jtype jversion'=. (3&{,{.) <;._2 ,&'/' 9!:14''"
              "basedir=. ({.~ _2 { I.@:=&'/') BINPATH"

              "share=.  basedir,'/share/j'"
              "system=. share,'/system'"
              "tools=.  share,'/tools'"

              "user=.    home,'/.config/j/',jversion"
              "addons=.  user,'/addons'"
              "break=.   user,'/break'"
              "config=.  user,'/config'"
              "install=. user,'/install'"
              "snap=.    user,'/snap'"
              "temp=.    user,'/temp'")
            "\n"))))))

(define* (j-package #:key
                    (version (git-version))
                    (release-type 'release)
                    (extra-inputs '())
                    (extra-envars '())
                    (toolchain clang-toolchain)
                    (compiler-path "/bin/clang")
                    (builder (git-user)))
  (package
    (name "j")
    (version version)
    (source (local-file %source-dir #:recursive? #t #:select? git-file?))
    (build-system gnu-build-system)
    (native-inputs `(("toolchain" ,toolchain)))
    (inputs (cons* libedit libomp jsoftware-auxiliary-files extra-inputs))
    (arguments
     `(#:parallel-build? #f
       #:parallel-tests? #f
       #:tests? #f
       #:modules (((ice-9 ftw) #:select (scandir))
                  ((ice-9 popen) #:select (open-pipe* close-pipe))
                  ((ice-9 regex) #:select (match:substring string-match))
                  ((ice-9 threads) #:select (par-for-each parallel))
                  ((srfi srfi-26) #:select (cut))
                  ((srfi srfi-1) #:select (fold))
                  ,@%gnu-build-system-modules)
       #:phases
       ;; Upstream's build system consists of ad-hoc scripts that build build
       ;; up (very complicated) environment variables to pass to make.  The
       ;; basic build process looks like this:
       ;;
       ;;   1) Copy jsrc/jversion-x.h to jsrc/jversion.h and edit values;
       ;;   2) Set jplatform and j64x environment variables;
       ;;   3) Run make3/build_jconsole.sh and make3/build_libj.sh;
       ;;
       ;; However, upstream expects users to run J directly from the source
       ;; directory; they do not supply a make `install' target.  Thus it takes
       ;; some massaging to install files in FHS-style directories.

       (modify-phases %standard-phases
         ;; In particular, we have to set up
         ;;
         ;;   1) jsrc/jversion.h as in a typical build;
         ;;   2) jlibrary/bin/profilex.ijs to point to writable directories;
         ;;   3) make3/build_*.sh to respect standard build conventions;
         ;;   4) jsrc/jconsole.c to fix libedit dlopen; and
         ;;   5) Hard coded references to addons directory.
         (replace 'configure
           (lambda* (#:key target inputs outputs #:allow-other-keys)
             (let* ((toolchain (assoc-ref inputs "toolchain"))
                    (compiler (string-append toolchain ,compiler-path))
                    (libedit (assoc-ref inputs "libedit"))
                    (out (assoc-ref outputs "out")))
               ;; Set up build constants
               (copy-file "jsrc/jversion-x.h" "jsrc/jversion.h")
               (substitute* "jsrc/jversion.h"
                 (("^#define jversion.*$")
                  (format #f "#define jversion ~s\n" ,version))
                 (("^#define jtype.*$")
                  (format #f "#define jtype ~s\n" "debug"))
                 (("^#define jbuilder.*$")
                  (format #f "#define jbuilder ~s\n" ,builder)))
               ;; Munge the build scripts into reason:
               ;; 1. Short-circuit the fragile compiler detection;
               ;; 2. Make sure to include our CFLAGS and LFLAGS; and
               ;; 3. Propagate script errors to top level.
               (with-directory-excursion "make3"
                 (for-each
                  (lambda (file)
                    (substitute* file
                      ;; The `compiler' variable doesn't point to the actual
                      ;; compiler.  It is just a switch to tell the build
                      ;; scripts whether to use gcc- or clang-specific flags.
                      (("^compiler=.*$") "compiler=clang\n")
                      (("^LDFLAGS=\"" def) (string-append def "$LDFLAGS "))
 
                      ;; Prepend our $CFLAGS to 'common', which holds CFLAGS
                      ;; common between all compilers. Note: We are careful to
                      ;; only add these flags once, ignoring lines like
                      ;; common="$common ...". The __GUARD__ dance works around
                      ;; the lack of regex negative lookahead.
                      (("^common=\"\\$common") "__GUARD__")
                      (("^(common=\")(.*)$" _ def rest)
                       (string-append def "$CFLAGS " rest))
                      (("^__GUARD__") "common=\"$common")
 
                      ;; Make sure errors exit with non-zero exit code.
                      (("^exit\n$") "exit 1\n")
 
                      ;; Allow build scripts to pass arguments to make. The
                      ;; scripts pass a large number of variables to make, and
                      ;; the build step needs to invoke the `clean' target.
                      (("^(make -f .*)\n$" _ make-cmd)
                       (string-append make-cmd " \"$@\"\n"))
 
                      (("^#!.*" shebang)
                       (string-append shebang "set -o errexit\n"))))
                  '("build_jconsole.sh" "build_libj.sh")))
               ;; The jconsole manually loads libedit with dlopen.  The path
               ;; must be absolute to correctly point to our input.
               (substitute* "jsrc/jconsole.c"
                 (("libedit\\.so\\.[0-9]" so-file)
                  (format #f "~a/lib/~a" libedit so-file)))
               ;; The ~addons/dev directory supplies tentative J-script
               ;; definitions of new J engine functionality.  Since we point
               ;; ~addons under the ~user directory, we move it under ~system
               ;; instead, which sits as-is in the output.
               (with-directory-excursion "jsrc"
                 (for-each
                   (lambda (file)
                     (substitute* file (("~addons/dev") "~system/dev")))
                   (scandir "."
                     (lambda (f) (eq? (stat:type (stat f)) 'regular)))))
               ;; Implementation of 9!:14 records build time which breaks build
               ;; reproducibility.  Note that upstream code depends on the
               ;; exact format of these strings, so we need to mimic the
               ;; standard.
               (substitute* "jsrc/j.c"
                 (("__DATE__") "\"Jan 01 1970\"")
                 (("__TIME__") "\"00:00:00\""))
               ;; Upstream recommends using clang, with GCC support being
               ;; second-class, often resulting in build failures.
               (setenv "CC" compiler))))
 
         ;; The build output depends primarily on the values of the `jplatform'
         ;; and `j64x' environment variables.  If the target is ARM, then
         ;; `jplatform' is "raspberry", otherwise it is `linux'.  In addition
         ;; to 32- and 64- bit versions, `j64x' controlls whether AVX or AVX2
         ;; variants of libj are built.
         ;;
         ;; However, build targets are not fine-grained enough to distinguish
         ;; between CPU features.  Thus we build and install all variants of
         ;; libj, expecting jconsole to be called with a wrapper script that
         ;; detects AVX features and loads the appropriate libj at runtime.
         (replace 'build
           (lambda* (#:key parallel-build? #:allow-other-keys)
             (setenv "CFLAGS" "-g")
             (setenv "USE_OPENMP" "1")
             (setenv "USE_THREAD" "1")
             (for-each (lambda (var-val) (apply setenv var-val))
                       (quote ,extra-envars))
             ;; The build scripts assume that PWD is make3.
             (with-directory-excursion "make3"
               (let* ((platform ,(if (target-arm?) "raspberry" "linux"))
                      (target-bit ,(if (target-64bit?) "64" "32"))
                      (run-script
                        (lambda (script platform jbit . args)
                          (apply invoke
                                 (cons* "env"
                                        (string-append "jplatform=" platform)
                                        (string-append "j64x=" jbit)
                                        script args))))
                      (run (lambda* (script #:key (variant ""))
                             (let* ((jbit (string-append "j" target-bit variant))
                                    (m (string-match "build_(.*)\\.sh" script))
                                    (c (match:substring m 1))
                                    (makefile (string-append "makefile-" c)))
                               (begin
                                 ;; Scripts use `jplatform' and `j64x'
                                 ;; environment variables to determine what
                                 ;; binary to produce.
                                 (run-script script platform jbit)
                                 ;; Object files lingering around from previous
                                 ;; builds will incorrectly propagate to
                                 ;; others.
                                 (run-script script platform jbit "clean"))))))
                 (let-syntax ((run-steps
                                (syntax-rules ()
                                  ((run-steps exp ...)
                                   (if parallel-build?
                                     (parallel exp ...)
                                     (begin exp ...))))))
                   (run-steps
                     ;; Since jconsole doesn't depend on AVX features, we just
                     ;; build it once.
                     (run "./build_jconsole.sh")
                     (run "./build_libj.sh")
                     (run "./build_tsdll.sh")
                     (if ,(target-64bit?)
                       (run-steps
                         (run "./build_libj.sh" #:variant "avx")
                         (run "./build_tsdll.sh" #:variant "avx")
                         (run "./build_libj.sh" #:variant "avx2")
                         (run "./build_tsdll.sh" #:variant "avx2")))))))))
         ;; The test suite is expected to be run as follows for each variant of
         ;; libj that we build:
         ;;
         ;;     $ echo 'RUN ddall' | jconsole test/tsu.ijs
         ;;
         ;; This requires a working jconsole with accessible jlibrary files. We
         ;; simply place these all under test/bin.
         (replace 'check
           (lambda* (#:key tests? parallel-tests? #:allow-other-keys)
             (when tests?
               (let ((platform ,(if (target-arm?) "raspberry" "linux"))
                     (for-each* (if parallel-tests? par-for-each for-each)))
                 (mkdir-p "test/bin")
                 (for-each
                   (lambda (dir)
                     (let ((source (string-append "jlibrary/" dir))
                           (dest (string-append "test/bin/" dir)))
                     (begin
                       (mkdir-p dest)
                       (copy-recursively source dest))))
                   '("system" "tools" "addons"))
                 ;; The jlibrary/dev directory only sometimes exists, but needs
                 ;; to be copied into the ~system directory when it does.
                 (for-each
                   (lambda (dev-dir)
                     (if (file-exists? dev-dir)
                       (copy-recursively dev-dir "test/bin/system/dev")))
                   '("jlibrary/dev" "jlibrary/addons/dev"))
                 (for-each*
                   (lambda (dir)
                     (let* ((bin (string-append "bin/" platform))
                            (jbit ,(if (target-64bit?) "j64" "j32"))
                            (jconsole (string-append bin "/" jbit "/jconsole"))
                            (source (string-append bin "/" dir))
                            (dest (string-append "test/bin/" dir)))
                       (begin
                         (mkdir-p dest)
                         (copy-recursively source dest)
                         (install-file "jlibrary/bin/profile.ijs" dest)
                         (install-file jconsole dest)
                         ;; Some tests verify firesystem interaction and
                         ;; require the ability to create ~user, which normally
                         ;; resides under HOME. Note, this path should be
                         ;; unique to each test run, to prevent collisions
                         ;; between tests.
                         (setenv "HOME" (string-append (getcwd) "/" dest))
                         (let* ((jconsole (string-append dest "/jconsole"))
                                (tests "test/tsu.ijs")
                                (port (open-pipe* OPEN_WRITE jconsole tests)))
                           ;; This input to tsu.ijs runs the tests, ensuring
                           ;; that failures induce a non-zero exit while also
                           ;; printing out the name of the failed tests.
                           (display "(exit@# [ echo) RUN ddall\n" port)
                           (when (not (zero? (status:exit-val
                                               (close-pipe port))))
                             (error "Some J build tests failed."))))))
                   (scandir (string-append "bin/" platform)
                            (negate (cut member <> '("." "..")))))
                 #t))))
         ;; Now that everything is built, installation is fairly
         ;; straightforward, following FHS conventions.  The only quirk is that
         ;; we install jconsole under /libexec to make room for the wrapper
         ;; replacement under /bin.
         (replace 'install
           (lambda* (#:key outputs inputs #:allow-other-keys)
             (let* ((platform ,(if (target-arm?) "raspberry" "linux"))
                    (jbit ,(if (target-64bit?) "j64" "j32"))
                    (out (assoc-ref outputs "out"))
                    (bin (string-append out "/bin"))
                    (etc (string-append out "/etc/j"))
                    (lib (string-append out "/lib/j"))
                    (libexec (string-append out "/libexec/j"))
                    (share (string-append out "/share/j"))
                    (system (string-append share "/system"))
                    (dev (string-append system "/dev")))
               (mkdir-p bin)
               (copy-file (search-input-file inputs "bin/ijconsole")
                          (string-append bin "/ijconsole-" ,version))
               (mkdir-p lib)
               (for-each
                 (lambda (jarch)
                   (let* ((jbin (string-join `("bin" ,platform ,jarch) "/"))
                          (javx-match (string-match "avx.*" jarch))
                          (javx (if (not javx-match) ""
                                  (match:substring javx-match)))
                          (sep (if javx-match "-" ""))
                          (source (string-append jbin "/libj.so"))
                          (dest (format #f "~a/libj~a~a.so" lib sep javx)))
                     (copy-file source dest)))
                 (scandir (string-append "bin/" platform)
                          (negate (cut member <> '("." "..")))))
               (install-file (string-append "bin/" platform
                                            "/" jbit "/jconsole")
                             libexec)
               (copy-recursively "jlibrary/system" system)
               (for-each
                 (lambda (source-dev)
                   (if (access? source-dev R_OK)
                     (copy-recursively source-dev dev)))
                 '("jlibrary/dev" "jlibrary/addons/dev"))
               (install-file "jlibrary/bin/profile.ijs" etc)
               (install-file (search-input-file inputs "etc/j/profilex.ijs")
                             etc)))))))
    (home-page "https://www.jsoftware.com/")
    (synopsis "Ascii-only, array programming language in the APL family")
    (description
     "J is a high-level, general-purpose programming language that is
particularly suited to the mathematical, statistical, and logical analysis of
data.  It is a powerful tool for developing algorithms and exploring problems
that are not already well understood.")
   (license license:gpl3+)))


;;; Execution
(define (path-hash-result path target-hash)
  (let* ((hash (bytevector->base32-string (file-hash* path))))
    (list path target-hash hash (equal? hash target-hash))))

(define (build-package package)
  (run-with-store (open-connection)
    (mlet* %store-monad
      ((drv (package->derivation package))
       (grafts ((store-lift package-grafts) package)))
      (mbegin %store-monad
        (built-derivations (list drv))
        (return (map (compose derivation-output-path cdr)
                     (derivation-outputs drv)))))))

(define (build-packages package . packages)
  (concatenate (map build-package (cons package packages))))

(define (print-build-results paths target-hashes)
  (for-each
    (match-lambda
      ((path target-hash hash match?)
       ;(format #t "~a\n\t~a\t~a\n\t\t~a\n" path match? target-hash hash)
       (display
         (string-append
           (colorize-string path (color DARK)) "\n"
           "\t" (colorize-string "Target Hash: " (color DARK)) target-hash "\n"
           "\t" (colorize-string "Actual Hash: " (color DARK)) hash "\n"
           "\t" (colorize-string "Hashes Match? " (color DARK))
           (if match?
             (colorize-string "YES" (color GREEN))
             (colorize-string "NO" (color RED)))
           "\n"))))
    (map path-hash-result paths target-hashes)))

(define j-base-args
  `(#:toolchain ,clang-toolchain-12
    #:extra-inputs ,(list sleef)
    #:extra-envars (("USE_SLEEF_SRC" "0")
                    ("LDFLAGS" "-lsleef"))))

(define j (apply j-package j-base-args))
(define %j-target-hash "har6llk2rv3bwh3gao3qr3ue3dkmm6n4agvmkjhkes3iku2zjngq")

(match (program-arguments)
  ;; Don't execute anything if loading in repl
  ((cmd "repl" . args) #t)
  ;; Build and show results if executing as a script
  ((? (lambda (args)
        (equal? (canonicalize-path (car args)) (current-filename))))
   (print-build-results (build-packages j) (list %j-target-hash)))
  ;; Otherwise, assume we're feeding to a guix command
  (_ j))
