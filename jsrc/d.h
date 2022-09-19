/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Debug                                                                   */


// values returned in box 0 of an AFDEBUGRESULT value - big enough to be an integer

#define SUSCLEAR    7       /* clear stack                                  */
#define SUSRUN      2       /* run again                                    */
#define SUSRET      3       /* return result                                */
#define SUSJUMP     4       /* jump to specified line                       */
#define SUSNEXT     5       /* run next line                                */
#define SUSSS       6       /* single step mode                             */
#define SUSTHREAD   8       // set thread number


/* si->dcss values; 0 means not single step                                 */

#define SSCUTBACK   19      /* cut back one step level                      */
#define SSSTEPOVER  20      /* run curr line; stop at next line in curr fn  */
#define SSSTEPINTO  21      /* run curr line; stop at next line             */
#define SSSTEPOUT   22      /* run curr fn to end; stop at next line        */
#define SSSTEPOVERs 30      // stop at next line in this fn
#define SSSTEPINTOs 31      // stop at next line


extern B        jtdbstop(J,DC,I);
extern DC       jtssnext(J,DC,C);
extern I        lnumcw(I,A);
extern I        lnumsi(DC);

// set the stack entry pointed to by s to indicate resumption at line n
#define DGOTO(s,n) { (s)->dcix=(n); (s)->dcnewlineno=1; }  // set line# & indicate to use it


