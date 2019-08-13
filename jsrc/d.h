/* Copyright 1990-2002, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Debug                                                                   */


/* jt->uflags.us.cx.cx_c.db and jt->dbuser values; 0 means no debug                           */

#define DB1         1       /* full debug                                   */


/* jt->dbsusact values:                                                     */

#define SUSCONT     0       /* continue current suspension                  */
#define SUSCLEAR    1       /* clear stack                                  */
#define SUSRUN      2       /* run again                                    */
#define SUSRET      3       /* return result                                */
#define SUSJUMP     4       /* jump to specified line                       */
#define SUSNEXT     5       /* run next line                                */
#define SUSSS       6       /* single step mode                             */


/* si->dcss values; 0 means not single step                                 */

#define SSCUTBACK   19      /* cut back one step level                      */
#define SSSTEPOVER  20      /* run curr line; stop at next line in curr fn  */
#define SSSTEPINTO  21      /* run curr line; stop at next line             */
#define SSSTEPOUT   22      /* run curr fn to end; stop at next line        */
#define SSSTEPOVERs 30
#define SSSTEPINTOs 31


extern B        jtdbstop(J,DC,I);
extern DC       jtssnext(J,DC,C);
extern I        lnumcw(I,A);
extern I        lnumsi(DC);

// set the stack entry pointed to by s to indicate resumption at line n
#define DGOTO(s,n) { (s)->dcix=(n); (s)->dcnewlineno=1; }  // set line# & indicate to use it
