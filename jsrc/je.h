/* Copyright (c) 1990-2025, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Extern Declarations                                                     */
#include "gemm.h"

extern DF2(atcomp);
extern DF1(fork120);
extern F1(jtabase1);
extern DF1(jtadot1);
extern F1(jtadotidot);
extern F1(jtarep);
extern F1(jtaro);
extern DF1(jtarx);
extern F1(jtasgzombq);
extern F1(jtasgzombs);
extern F1(jtdeprecxq);
extern F1(jtdeprecxs);
extern F1(jtassertq);
extern F1(jtasserts);
extern F1(jtecmtriesq);
extern F1(jtecmtriess);
// extern F1(jtdirectdefq);
// extern F1(jtdirectdefs);
extern F1(jtaudittdisab);
extern F1(jtbase1);
extern F1(jtbdot);
extern F1(jtbehead);
extern F1(jtbinrep1);
// extern F1(jtbitadv);
extern F1(jtbox);
extern F1(jtboxedsparse);
extern F1(jtboxopen);
extern F1(jtboxq);
extern F1(jtboxs);
extern F1(jtbreakfnq);
extern F1(jtbreakfns);
extern F1(jtbsdot);
extern F1(jtbslash);
extern F1(jtca);
extern F1(jtcaro);
extern F1(jtcallback);
extern F1(jtcallbackx);
extern DF1(jtcant1);
// extern F1(jtcar);
extern F1(jtcasev);
extern DF1(jtcatalog);
extern F1(jtcddlclose);
extern F1(jtcder);
extern F1(jtcderx);
extern F1(jtcdf);
extern F1(jtcdjt);
extern F1(jtcdlibl);
extern F1(jtcddlopen);
extern DF1(jtcdot1);
extern F1(jtcdproc1);
extern F1(jtcdq);
// extern F1(jtceil1);
extern F1(jtcerrno);
extern F1(jtcheckfreepool);
extern F1(jtsetgetrecurstate);
extern F1(jtcallJDo);
extern DF1(jtcompsum);
// extern F1(jtconjug);
extern F1(jtcpufeature);
extern F1(jtcrc1);
extern F1(jtcrccompile);
extern F1(jtcreatecachedref);
extern F1(jtctq);
extern F1(jtcts);
extern F1(jtcurtail);
extern F1(jtcurnameq);
#if !C_CRC32C
extern F1(jtcvt0);
#endif
extern F1(jtdbc);
extern F1(jtdbcall);
extern F1(jtdbcutback);
extern F1(jtdberr);
extern F2(jtdberr2);
extern F1(jtdbetx);
extern F1(jtdbjump);
extern F1(jtdbnext);
extern F1(jtdbq);
extern F1(jtdbret);
extern F1(jtdbrr1);
extern F1(jtdbrun);
extern F1(jtdbsig1);
extern F1(jtdbstack);
extern F1(jtdbstackz);
extern F1(jtdbstepinto1);
extern F1(jtdbstepout1);
extern F1(jtdbstepover1);
extern F1(jtdbstopq);
extern F1(jtdbstops);
extern F1(jtdbtrapq);
extern F1(jtdbtraps);
extern F1(jtdecrem);
extern F1(jtdenseit);
extern DF1(jtdet);
extern F1(jtdgrade1);
extern DF1(jtdigits10);
extern F1(jtdispq);
extern F1(jtdisps);
extern F1(jtdl);
extern F1(jtdllsymdat);
extern F1(jtdllvaladdr);
extern F1(jtdllsymhdr);
extern F1(jtdllsymget);
extern F1(jtdllsymset);
extern F1(jtvalenceerr);
extern F1(jtdrep);
extern DF1(jtdrx);
extern F1(jtduble);
extern F1(jtdx_test);
extern F1(jtemend);
extern F1(jtevmq);
extern F1(jtevms);
extern F1(jtex);
extern DF1(jtexec1);
extern F1(jtexg);
// extern F1(jtexpn1);
// extern F1(jtfact);
extern F1(jtfactor);
extern F1(jtfh15);
extern A jtfiller(J,I,I,I*);
extern DF1(jtfix);
// extern F1(jtfloor1);
extern F1(jtfmt01);
extern F1(jtfmt11);
extern F1(jtfmt21);
extern F1(jtfname);
extern DF1(jtfrombase64);
extern F1(jtfsmvfya);
extern F1(jtfullname);
extern DF1(jtfx);
extern DF1(jtfxeach);
extern PRIM jtfxself[2];
extern F1(jtfxx);
extern F1(jtgaussdet);
extern DF1(jtgausselm);
extern F1(jtgb_test);
extern F1(jtgemmtune);
extern DF1(jtemsglevel);
extern F1(jtgh15);
extern F1(jtgrade1);
extern F1(jtgr1);
extern F1(jtgsignal);
extern A jtkeyct(J,A,A,A,D);
extern DF2(jtkeybox);
extern DF2(jtkeyheadtally);
extern F2(jtkeytally);
extern F1(jthalve);
extern F1(jthash);
extern F1(jthead);
extern F1(jthexrep1);
// extern F1(jthgdiff);
extern DF1(jthost);
extern DF1(jthostne);
extern DF1(jticap);
extern F1(jticvt);
extern F1(jtiden);
extern F1(jtidensb);
extern F1(jtiepdoq);
extern F1(jtiepdos);
extern F1(jtiepq);
extern F1(jtieps);
extern F1(jtimmea);
extern F1(jtimmex);
extern F1(jtincrem);
extern A jtintmod2(J,A,I);
extern DF1(jtintfloorlog2cap);
extern DF1(jtintceillog2cap);
extern F1(jtinttoe);
extern F1(jtetoint);
extern DF1(jtiota);
extern F1(jtisnan);
extern F1(jtisempty);
extern F1(jtisitems);
extern F1(jtisnotempty);
extern F1(jtjclose);
extern F1(jtjdir);
extern F1(jtjdot1);
extern DF1(jtjfatt1);
extern DF1(jtjferase);
extern F1(jtjfiles);
extern DF1(jtjfperm1);
extern DF1(jtjfread);
extern DF1(jtjfsize);
extern DF1(jtjgetenv);
extern F1(jtjgetx);
extern F1(jtjgetpid);
extern DF1(jtjico1);
extern F2(jtindaudit);
extern DF1(jtjiread);
extern DF1(jtjlock);
extern F1(jtjlocks);
extern DF1(jtjmkdir);
extern F1(jtnatoms);
extern F1(jtjoff);
extern F1(jtjopen);
extern F1(jtjpr);
extern F1(jtrank);
// extern F1(jtjregcomp);
// extern F1(jtjregerror);
// extern F1(jtjregfree);
// extern F1(jtjreghandles);
// extern F1(jtjreginfo);
extern DF1(jtjunlock);
extern F1(jtlamin1);
extern F1(jtlcg_test);
extern F1(jtlevel1);
extern F1(jtloccre1);
extern DF2(jtlocexmark);
extern F1(jtlock1);
extern F1(jtlocmap);
extern F1(jtlocname);
extern F1(jtlocnc);
extern F1(jtlocnl1);
extern F1(jtlocnlz1);
extern F1(jtlochdr);
extern DF1(jtlocpath1);
extern F1(jtlocsizeq);
extern F1(jtlocsizes);
extern F1(jtlocswitch);
// extern F1(jtlogar1);
extern F1(jtlrep);
extern DF1(jtlrx1);
extern DF2(jtlrx2);
extern F1(jtltrim);
extern DF2(jtludecomp);
extern F1(jtresetbloom);
// extern F1(jtmag);
extern F1(jtmap);
extern F1(jtmat);
extern F1(jtmema);
extern DF2(jtmemalign);
extern F1(jtmemf);
extern F1(jtmemo);
extern F1(jtmemr);
extern F1(jtmemu);
extern F1(jtminv);
extern F1(jtmmaxq);
extern F1(jtmmaxs);
extern F1(jtmr_test);
extern F1(jtmt_test);
extern F1(jtmvmsparse);
extern F1(jtmvmsparsee);
extern A jtnameref(J,A,A);
extern F1(jtnmcacheq);
extern F1(jtnmcaches);
extern F1(jtnc);
extern F1(jtnch);
extern DF1(jtnegate);
extern F1(jtnfb);
extern F1(jtnfes);
extern F1(jtnl1);
extern A jtnlsym(J,A,A,I);
extern F1(jtnot);
extern F1(jtnub);
extern DF1(jtnubind);
extern DF1(jtnubind0);
extern DF2(jtepsind);
extern DF2(jtepsind0);
extern F1(jtnubsieve);
extern F1(jtnubsievesp);
extern F1(jtnulljob);
extern F1(jtnulljob);
extern DF1(on1);
extern DF1(on1cell);
extern F1(jtonm);
extern F1(jtope);
extern F1(jtoutparmq);
extern F1(jtoutparms);
extern F1(jtparse);
extern F1(jtparsercalls);
extern F1(jtpathchdir);
extern F1(jtpathcwd);
extern F1(jtpathdll);
extern DF1(jtpderiv1);
extern F1(jtpeekdata);
extern DF1(jtpinv);
extern DF1(jtpix);
extern F1(jtplt);
extern F1(jtpmarea1);
extern F1(jtpmctr);
extern F1(jtpmstats);
extern F1(jtpmunpack);
extern F1(jtpolar);
extern DF1(jtpoly1);
extern F1(jtposq);
extern F1(jtposs);
extern F1(jtpparity);
extern F1(jtppq);
extern F1(jtpps);
extern F1(jtprep);
extern F1(jtprime);
extern F1(jtprocarch);
extern F1(jtprocfeat);
extern DF1(jtprx);
extern F1(jtqpctr);
extern F1(jtqpfreq);
extern DF1(jtqr);
extern F1(jtquerybloom);
extern F1(jtranking);
extern F1(jtrankle);
extern F1(jtrat);
extern F1(jtravel);
extern F1(jtraze);
extern F1(jtrazeh);
extern F1(jtrazein);
extern F1(jtrca);
extern F1(jtrdot1);
extern DF1(jtrecip);
extern DF1(jtrect);
extern F2(jtredefg);
extern F1(jtretcommq);
extern F1(jtretcomms);
extern F1(jtreverse);
extern F1(jtright1);
extern DF1(jtrinv);
extern F1(jtrngraw);
extern F1(jtrngseedq);
extern F1(jtrngseeds);
extern F1(jtrngselectq);
extern F1(jtrngselects);
extern F1(jtrngstateq);
extern F1(jtrngstates);
extern F1(jtroll);
extern F1(jtrollx);
extern F1(jtrtrim);
extern F1(jtsb1);
extern F1(jtsborder);
extern F1(jtscind);
extern F1(jtscriptstring);
extern F1(jtscriptnum);
extern F1(jtsclass);
extern DF1(jtscm00);
extern DF1(jtscm01);
extern DF1(jtscm10);
extern DF1(jtscm11);
extern DF1(jtsct1);
extern DF1(jtscy1);
extern DF1(jtscz1);
extern F1(jtseclevq);
extern F1(jtseclevs);
extern F1(jtshape);
extern F1(jtshapex);
extern F1(jtshasum1);
extern F1(jtshift1);
// extern F1(jtsignum);
extern F1(jtsiinfo);
extern F1(jtslash);
extern F1(jtsldot);
extern F1(jtsldotdot);
extern F1(jtsnl);
extern F1(jtsp);
extern F1(jtsparse1);
extern F1(jtspres);
extern F1(jtspcount);
extern F1(jtspfor);
extern F1(jtspforloc);
extern F1(jtsphwmk);
extern DF1(jtspit);
extern F1(jtspresident);
extern F1(jtspallthreads);
// extern F1(jtsqroot);
extern F1(jtsquare);
extern F1(jtstackfault);
extern F1(jtstdnm);
extern F1(jtstr0);
extern F1(jtstype);
extern F1(jtswap);
extern F1(jtsymbrd);
extern F1(jtsymbrdlock);
extern F1(jtsymbrdlocknovalerr);
extern F1(jtsympool);
extern F1(jtsysparmq);
extern F1(jtsysparms);
extern F1(jtsysq);
extern F1(jttable);
extern F1(jttail);
extern F1(jttally);
// extern F1(jttco);
extern F2(jttdot);
extern F1(jttcapdot1);
extern F2(jttcapdot2);
extern F1(jtthorn1);
extern F1(jtthorn1u);
extern A jtthorn1xy(J,A,I);
extern DF1(jttobase64);
extern F1(jttocesu8);
extern F1(jttomutf8);
extern F1(jttou32);
extern F1(jttoutf16);
extern F1(jttoutf16x);
extern F1(jttoutf32);
extern F1(jttoutf8);
extern A jttoutf8a(J,A,A);
extern F1(jttoutf8x);
extern F1(jttrep);
extern DF1(jttrx);
extern F1(jtts);
extern F1(jtts0);
extern F1(jttsit1);
extern F1(jttss);
extern F1(jtuco1);
extern F1(jtunbin);
extern F1(jtmemhistoq);
extern F1(jtmemhistos);
extern F1(jtmemhashq);
extern F1(jtmemhashs);
void memhashadd(I,C*);
extern F1(jtunlock1);
extern A jtunname(J,A,I);
extern F1(jtunzero);
extern F1(jtvaspz);
extern F1(jtversq);
extern F1(jtvi);
extern F1(jtvib);
extern F1(jtvip);
extern F1(jtvs);
extern F1(jtvslit);
extern F1(jtvtrans);
extern F1(jtwordil);
extern DF1(jtwords);
extern F1(jtxco1);

// extern F1(jttest1);

extern DF2(jtabase2);
extern F2(jtadot2);
extern F2(jtadverse);
extern DF2(jtaes2);
// extern F2(jtagenda);
extern F2(jtagendai);
extern F2(jtamp);
extern F2(jtampco);
extern F2(jtanyebar);
extern DF2(jtapplystr);
extern F2(jtatco);
extern F2(jtatop);
extern F2(jtauditpyx);
extern F2(jtbase2);
extern F2(jtbinrep2);
// extern F2(jtbit);
// extern F2(jtbitmatch);
extern DF2(jtbitwiserotate);
extern DF2(jtbitwiseshift);
extern DF2(jtbitwiseshifta);
extern DF2(jtboxcut0);
extern F2(jtcallback2);
extern F2(jtcant2);
extern F2(jtcd);
extern F2(jtcddlsym);
extern F2(jtcdot2);
extern DF2(jtccapdot2);
extern F2(jtcdproc2);
extern F2(jtcolon);
extern DF2(jtcombineeps);
extern F2(jtcpufeature2);
extern F2(jtcrc2);
extern DF2(jtcut2);
extern F2(jtcut);
extern F2(jtdbrr2);
extern F2(jtdbsig2);
extern F2(jtdbstepinto2);
extern F2(jtdbstepout2);
extern F2(jtdbstepover2);
extern F2(jtdbpasss);
extern F1(jtdbisolatestk);
extern DF2(jtdeal);
extern DF2(jtdealx);
extern F2(jtdex);
extern F2(jtdgrade2);
extern F2(jtdot);
extern F2(jtdrop);
extern F2(jtebar);
extern F2(jtekupdate);
extern F2(jtqktupdate);
extern F1(jtfindspr);
extern F1(jtbatchop);
extern F2(jteps);
extern DF2(jtetoiso8601);
extern DF2(jtiso8601toe);
// extern F2(jteven);
extern F2(jtevger);
extern F2(jtexec2);
extern F2(jtexpand);
extern DF2(jtexpn2);
extern F2(jtfc2);
extern DF2(jtfetch);
extern F2(jtfit);
extern F1(jtqfill);
extern DF2(jtfitcteq);
extern F2(jtfixrecursive);
extern F2(jtfmt02);
extern F2(jtfmt12);
extern F2(jtfmt22);
extern DF2(jtfold);
extern DF1(jtfoldZ1);
extern DF2(jtfoldZ2);
extern DF2(jtfoldZ);
extern DF2(jtfolk2);
extern F2(jtforeign);
extern F2(jtforeignextra);
extern DF2(jtfrom);
extern F2(jtfrombs);
extern F2(jtfromis);
extern F2(jtfromr);
extern F2(jtfromsd);
extern F2(jtfromss);
extern F2(jtfsm);
extern F2(jtgemmtune2);
// extern F2(jtgenbitwiserotate);
// extern F2(jtgenbitwiseshift);
// extern F2(jtgenbitwiseshifta);
extern F2(jtgrade1p);
extern F2(jtgrade2);
extern F2(jthexrep2);
extern F2(jthgeom);
extern A jthook(J,A,A,A);
extern F2(jti1ebar);
extern F2(jtic2);
extern DF2(jticap2);
extern DF2(jtifbebar);
extern F2(jtifrom);
extern F2(jtindexof);
extern F2(jtintdiv);
extern F2(jtjdot2);
extern DF2(jtjfappend);
extern DF2(jtjfatt2);
extern DF2(jtjfperm2);
extern DF2(jtjfwrite);
extern F2(jtjico2);
extern DF2(jtjiwrite);
// extern F2(jtjregmatch);
// extern F2(jtjregmatches);
extern DF2(jtlamin2);
extern F2(jtlcapco);
extern F2(jtleft2);
extern F2(jtless);
#if C_CRC32C && SY_64
extern DF2(jtintersect);
#endif
extern F2(jtlev);
extern DF2(jtjlink);
extern F2(jtloccre2);
extern F2(jtlock2);
extern F2(jtlocnl2);
extern DF2(jtlocpath2);
extern F2(jtlogar2);
extern F2(jtlowerupper);
extern F2(jtlrtrim);
extern F2(jtmatch);
extern DF2(jtmdiv);
extern F2(jtmdot);
extern F2(jtmemu2);
extern F2(jtmemw);
extern F2(jtnamerefop);
extern F2(jtnl2);
extern F2(jtnotmatch);
extern F2(jtnouninfo2);
extern F2(jtobverse);
// extern F2(jtodd);
extern I jtoldsize(J,X);
extern DF2(jtordstat);
extern DF2(jtordstati);
extern F2(jtoutstr);
extern DF2(jtover);
extern F2(jtpco2);
extern DF2(jtpderiv2);
extern F2(jtpdt);
extern F2(jtpdtsp);
extern F2(jtpmarea2);
extern DF2(jtpoly2);
extern F2(jtqco2);
extern DF2(jtqhash12);
extern F2(jtqq);
extern F2(jtrdot2);
extern F2(jtreaxis);
extern F2(jtreitem);
extern F2(jtrepeat);
extern F2(jtrepeatr);
extern F2(jtreshape);
extern DF2(jtresidue);
extern F2(jtrezero);
extern F2(jtright2);
extern F2(jtroot);
extern F2(jtrotate);
extern F2(jtsb2);
extern F2(jtscapco);
extern DF2(jtscm002);
extern DF2(jtscm012);
extern DF2(jtscm102);
extern DF2(jtscm112);
extern DF2(jtsct2);
extern DF2(jtscy2);
extern DF2(jtscz2);
extern DF2(jtself12);
extern F2(jtsetfv);
extern DF2(jtsfrom);
#if SY_64
extern F2(jtsfu);
#endif
extern DF2(jtshasum2);
extern F2(jtsmmcar);
extern F2(jtsmmis);
extern F2(jtsparse2);
extern DF2(jtspecialatoprestart);
extern DF2(jtstitch);
extern DF2(jtstitchsp2);
extern DF2(jtstringtoe);
extern F2(jtsumebar);
extern F2(jttake);
// extern F2(jttcap);
extern DF2(jtthorn2);
extern F2(jttie);
extern DF2(jttsit2);
extern F2(jtuco2);
extern F2(jtundco);
extern F2(jtunder);
extern F2(jtunlock2);
extern F2(jtunparsem);
extern DF2(jtupon2);
extern DF2(jtupon2cell);
extern F2(jtxco2);
extern DF2(jtxdefn);
extern F2(jtxlog2a);
extern F2(jtxroota);
extern DF2(jtxrx);

extern DF1(jtamend);
extern DF1(jtbitwise1);
extern DF1(jtbitwiseinsertchar);
extern DF1(jtcrcfixedleft);
extern DF1(jtdetxm);
extern DF1(jtexppi);
extern DF1(jtfsmfx);
extern DF1(jthgcoeff);
extern DF1(jthook1cell);
extern DF2(jthook2cell);
extern DF1(jtmean);
extern DF1(jtnum1);
extern DF1(jtrazecut1);
extern DF1(jtredcat);
extern DF1(jtredravel);
extern DF1(jtwd);

extern F2(jtapip);
extern DF1(jtatomic1);
extern DF2(jtatomic2);
extern DF2(jtbitwisechar);
extern DF2(jtcharfn2);
extern DF2(jtdeadband);
extern DF2(jtunquote);
extern A jtdbunquote(J,A,A,A);
extern DF2(jteachl);
extern DF2(jteachr);
extern DF2(jtfslashatg);
extern DF2(jtimplocref);
extern DF2(jtnum2);
extern DF2(jtpolymult);
extern DF2(jtpowop);
extern DF2(jtrazecut0);
extern DF2(jtrazecut2);
extern DF2(jtreshapeblankfn);
extern DF2(jtrollk);
extern F2(jtrollksub);
extern DF2(jtrollkx);
extern DF2(jtsumattymes1);
extern DF2(jtxop2);
#if AUDITEXECRESULTS
extern void auditblock (J jt,A w, I nonrecurok, I virtok);
#endif
#if FORCEVIRTUALINPUTS
extern A virtifnonip(J jt, I ipok, A buf);
#endif
extern A        jtassembleresults(J,I, A, A,A*,I,I,I,A,I,I,I);
extern void     audittstack(J);
extern I        cachedmmult(J,D*,D*,D*,I,I,I,I);
#if SY_64
extern void     copyTT(void *, void *, I, I,I);
#else
extern void     jtcopyTT(J, void *, void *, I, I,I);
#endif
extern A        cw57rep(J,A);
extern I        infererrtok(J);
extern I        johnson(I);
extern A        jtac1(J,AF);
extern A        jtac2(J,AF);
extern B        jtadd2(J,F,F,C*);
extern I        jtaii(J,A);
extern A        jtaindex(J,A,A,I);
extern A        jtam1a(J,A,A,A,B);
extern A        jtam1e(J,A,A,A,B);
extern A        jtam1sp(J,A,A,A,B);
extern A        jtamendn2(J,A,A,AD * RESTRICT,A);
extern A        jtamna(J,A,A,A,B);
extern A        jtamne(J,A,A,A,B);
extern A        jtamnsp(J,A,A,A,B);
extern A        jtapv(J,I,I,I);
extern A        jtapvwr(J,I,I,I);
extern A        jtascan(J,C,A);
extern A        jtaslash(J,C,A);
extern A        jtaslash1(J,C,A);
extern A        jtatab(J,C,A,A);
extern AF       jtatcompf(J,A,A,A);
extern void     jtauditmemchains(J);
extern B        jtb0(J,A);
extern A        jtbcvt(J,C,A);
extern B*       jtbfi(J,I,A,B);
extern D        jtbindd(J,D,D);
extern B        jtbitwisecharamp(J,UC*,I,UC*,UC*);
extern B        jtboxatop(J,A);
extern B        jtcdinit(JS);
extern DF1(jtcharmap);
extern B        jtchecksi(J);
extern A        jtclonelocalsyms(J,A);
extern A        jtclonevirtual(J,A);
extern I        jtcompare(J,A,A);
extern A        jtconnum(J,I,C*);
extern I        jtcountnl(J);
extern A        jtcpa(J,B,A);
extern A        jtcreatecycliciterator(J,A,A);
extern A        jtcrelocalsyms(J,A,A,I,I,I);
extern A        jtcstr(J,C*);
extern A        jtcvt(J,I,A);
extern A        jtccvt(J,I,A,I);
extern A        jtcvz(J,I,A);
extern A        jtdaxis(J,I,A);
extern A        jtddtokens(J,A,I);
extern DC       jtdeba(J,I,void*,void*,A);
extern void     jtdebdisp(J,DC);
extern void     jtdebz(J);
extern A        jtdecorate(J,A,I);
extern I        jtdeprecmsg(J,I,C*);
extern A        jtdfss1(J,A,A,A);
extern A        jtdfss2(J,A,A,A,A);
extern D        jtdgamma(J,D);
extern D        jtdgcd(J,D,D);
extern D        jtdlcm(J,D,D);
extern A        jtdropr(J,I,A);
extern B        jtecvt(J,D,I,int*,int*,C*);
extern B        jtecvtinit(J);
extern I        jtforeignassigninit(J);
extern A        jteformat(J,A,A,A,A);
extern A        jtenqueue(J,A,A,I);
extern B        jteqf(J,A,A);
extern B        jtequ(J,A,A);
extern B        jtequx(J,X,X);
extern B        jtequ0(J,A,A);
extern void     jterasenl(J,I);
extern A        jtev12(J,A,A,C*);
extern A        jteval(J,C*);
extern DF1(jtevery);
extern DF2(jtevery2);
extern DF1(jteveryself);
extern DF2(jtevery2self);
extern A        jtext(J,B,A);
extern A        jtexta(J,I,I,I,I);
extern I        jtextendunderlock(J,A*,US*,I);
extern L*       jtextnvr(J,L*);
extern void     jtfamf(J,AD * RESTRICT,I);
extern A        jtfdef(J,I,C,I,AF,AF,A,A,A,I,I,I,I);
extern A        jtfindnameinscript(J,C *,C *,I);
extern void     jtfillv0(J,I);        /* "fill" name conflict on Mac             */
extern A        jtfindnl(J,I);
extern I        jtfnum(J,A);
extern A        jtfolk(J,A,A,A);  /* "fork" name conflict under UNIX         */
extern void     jtforeigninit(J);
extern A        jtfrombsn(J,A,A,I);
extern A        jtfrombu(J,A,A,I);
extern A        jtpyxval(J,A);
extern C        jtjobrun(J,unsigned char(*)(J,void*,UI4),void*,UI4,I);
extern A        jtsusp(J,C);
extern A        jtfxeachv(J,I,A);
extern void*    jvmreserve(I);
extern B        jvmcommit(void*,I);
extern void*    jvmalloc(I);
extern void     jvmdecommit(void*,I);
extern void     jvmrelease(void*,I);
extern void*    jvmreservea(I,I);
extern void*    jvmalloca(I,I);
extern B        jvmwire(void*,I);
extern void     jvmunwire(void*,I);
#if SY_64
extern RESTRICTF A jtga0(J,I,I);
#else
extern RESTRICTF A jtga0(J,I,I,I);
#endif
extern void      jtrepatsend(J);
extern void      jtrepatrecv(J);
extern RESTRICTF A jtgaf(J,I);
extern RESTRICTF A jtgafv(J,I);
extern RESTRICTF A jtgah(J,I,A);
extern A        jtgc(J,A,A*);
extern I        jtgc3(J,A*,A*,A*,A*);
extern B        jtgerexact(J, A);
extern B        jtglobinit(JS);
extern B        jthasimploc(J,A);
extern I        jti0(J,A);
extern A        jtifb(J,I,B* RESTRICT);
extern I        jtigcd(J,I,I);
extern I        jtilcm(J,I,I);
extern A        jtincorp(J,A);
extern A        jtincorpra(J,A);
extern A        jtindexnl(J,I);
extern A        jtindexofprehashed(J,A,A,A,A);
extern A        jtindexofss(J,I,A,A);
extern A        jtindexofsub(J,I,A,A);
extern A        jtindexofxx(J,I,A,A);
extern A        jtintceillog2cap(J,A,A);
extern A        jtintfloorlog2cap(J,A,A);
extern A        jtinvamp(J,A,A,A);
extern A        jtiocol(J,I,A,A);
extern A        jtiovsd(J,I,A,A);
extern A        jtiovxs(J,I,A,A);
extern D        jtintpow(J,D,I);
extern A        jtirs1(J,A,A,I,AF);
extern A        jtirs2(J,A,A,A,I,I,AF);
extern I        jtisravelix(J,A);
extern A        jtjerrno(J);
extern A        jtjgets(JJ,C*);
extern C        jtjinit2(JS,int,C**);
extern C        jtjinitt(J);
extern A        jtinv(J,A,I);  // second arg is forced to 0 for initial call
extern F        jtjope(J,A,C*);
extern I        jtjset(J,C*,A);
extern A     jtjsigd(J,C*) __attribute__((cold));
extern A     jtjsignal(J,I) __attribute__((cold));
extern A     jtjsignale(J,I,A,I) __attribute__((cold));
extern void     jtjsignal2(J,I,A) __attribute__((cold));
extern A     jtjsignal3(J,I,A,I) __attribute__((cold));
extern A        jtleakblockread(J,A);
extern A        jtleakblockreset(J,A);
extern F1(jtshowinplacing1);
extern F1(jtcheckcompfeatures);
extern F2(jtshowinplacing2);
extern B        jtlocdestroy(J,A);
extern I        jtmaxtype(J,I,I);
extern B        jtmeminits(JS);
extern B        jtmeminitt(J);
extern void     jtmf(J, A,I,I);
extern A        jtmkwris(J, A);
extern I        jtmult(J,I,I);
extern A        jtnamerefacv(J, A, A);
extern A        jtnfs(J,I,C*);
extern A        jtodom(J,I,I,I* RESTRICT);
extern A        jtopenforassembly(J,A);
extern A        jtparsea(J,A*,I);
extern A        jtparsex(J,A*,I,I,DC);
extern A        jtpaxis(J,I,A);
extern A        jtpcvt(J,I,A);
extern A        jtpee(J,A*,UI8,I,I);
extern A        jtpfill(J,I,A);
extern A        jtpind(J,I,A);
extern B        jtpinit(J);
extern I        jtpiplocalerr(J, A);
extern void     jtpmrecord(J,A,A,I,int);
extern D        jtpospow(J,D,D);
extern B        jtpreparse(J,A,A*,A*);
extern B        jtprimitive(J,A);
extern A        probe(I,C*,L *,UI8);
extern B       jtprobedel(J,C*,UI4,A);
extern L*       jtprobeisres(J, A, A);
extern L*       jtprobeislocal(J,A,A);
extern A      jtprobequiet(J, A);
extern A       jtprobelocal(L*,A,A);
extern A       probelocalbuckets(L*,A,LX,I);
extern I        jtprod(J, I, I*);
extern I        jtqcompare(J,Q,Q);
extern A        jtra(AD* RESTRICT,I,A);
extern A        jtras(J,AD* RESTRICT);
extern A        jtra00s(J,AD* RESTRICT);
extern A        jtrank1ex(J,AD * RESTRICT,A,I,AF);
extern A        jtrank1ex0(J,AD * RESTRICT,A,AF);
extern A        jtrank2ex(J,AD * RESTRICT,AD * RESTRICT,A,UI,AF);
extern A        jtrank2ex0(J,AD * RESTRICT,AD * RESTRICT,A,AF);
extern A        jtrd(J,F,I,I);
extern A        jtrealize(J,A);
extern A        jtrealizeboxedvirtuals(J,A);
extern A        jtredcatcell(J,A,I);
extern D        jtremdd(J,D,D);
extern I        jtremid(J,I,D);
extern I        jtreservesym(J,I);
extern A        jtrifvs(J,AD* RESTRICT);
extern A        jtredef(J,A,A);
extern B        jtrnginit(J);
extern B        jtsbtypeinit(JS);
extern void     jtsbtypeglobinit();
extern A        jtsc(J,I);
extern A        jtsc4(J,I,I);
extern A        jtscansp(J,A,A,AF);
extern A        jtscb(J,B);
extern A        jtscc(J,C);
extern A        jtscf(J,D);
extern B        jtscheck(J,A);
extern A        jtscx(J,X);
extern A        jtscib(J,I);
extern B        jtsesminit(JS,I);
extern A        jtsetfv1(J,A,I);
#if PYXES
extern I jtsettaskrunning(J);
extern void jtclrtaskrunning(J);
#else
#define jtsettaskrunning(jt) ({I r=(jt->taskstate&TASKSTATERUNNING)==0; jt->taskstate|=TASKSTATERUNNING; r;})
#define jtclrtaskrunning(jt) (jt->taskstate&=~TASKSTATERUNNING)
#endif
extern void     jtsetleakcode(J,I);
extern A        jtsfn(J,B,A);
extern A        jtsfne(J,A);
extern void     jtshowerr(J);
extern A        jtsparseit(J,A,A,A);
extern I        jtspbytesinuse(J);
extern I        jtspstarttracking(J);
extern void     jtspendtracking(J);
extern B        jtspc(J);
extern A        jtspella(J,A);
extern A        jtspellcon(J,I);
extern A        jtspellout(J,C);
extern B        jtspfree(J);
extern B        jtspmult(J,A*,A,A,C,I,I,I,I);
extern A        jtsprank1(J,A,A,I,AF);
extern A        jtsprank2(J,A,A,A,I,I,AF);
extern A        jtssingleton1(J,A,I);
extern void     jtstackepilog(J,I4);
extern A        jtstcreate(J,I1,I,I,C*);
extern F        jtstdf(J,A);
extern A        jtstfind(J,I,C*,I);
extern A        jtstfindcre(J,I,C*,I);
extern A        jtstr(J,I,C*);
extern A        jtstrq(J,I,C*);
extern I        jtsumattymesprods(J,I,void *,void *,I,I,I,I,I,void *);
extern B        jtsymbinit(JS);
extern I        jtsymbis(J,A,A,A);
extern I        jtsymbisdel(J,A,A,A);
extern A        jtsymext(J);
extern void     jtsymfreeha(J,A);
extern L*       jtsymnew(J,LX*,LX);
extern void     jtsymreturn(J,LX,LX,I);
extern A        jtsybaseloc(J,A);
extern A       jtsyrd(J,A,A);
extern A       jtsyrd1(J,C*,UI4,A);
extern A        jtsyrdforlocale(J,A);
extern A        jtsyrd1forlocale(J,C*,UI4,A);
extern L*       jtsyrdfromloc(J,A,A);
extern A       jtsyrdnobuckets(J,A);
extern A        jtsystemlock(J,I,A (*)(JTT*));
extern I        jtsystemlockaccept(J,I);
extern A        jttaker(J,I,A);
extern D        jttceil(J,D);
extern D        jttfloor(J,D);
extern E        jteceil(J,E);
extern E        jtefloor(J,E);
extern I        jtthv(J,A,I,C*);
extern A        jttoc1(J,B,A);
extern void     jttoutf8w(J,C*,I,US*);
extern void       jttpop(J,A*,A*);
extern A*       jttpush(J,AD* RESTRICT,I,A*);
extern B        jttrd(J jt,A w);
extern A*       jttg(J,A*);
extern A        jttokens(J,A,I);
extern A        jtunDD(J,A);
extern B        jtunlk(J,I);
extern A        jtunparse(J,A,I);
extern A        jtunwordil(J,A,A,I);
extern A        jtv2(J,I,I);
extern A        jtva2s(J,A,A,C,VF,I,I,I,I,I);
extern VA2      jtvar(J,A,I,I);
extern A        jtvasp(J,A,A,C,VF,I,I,I,I,I,I,I,I,I);
extern B        jtvc1(J,I,US*);
extern A        jtvci(J,I);
extern A        jtvec(J,I,I,void*);
extern A        jtvecb01(J,I,I,void*);
extern F        jtvfn(J,F);
extern A        jtunvfn(J,F,A);
extern A        jtvger2(J,C,A,A);
extern A        jtvirtual(J,AD * RESTRICT,I,I);
extern B        jtvnm(J,I,C*);
extern void     jtwri(JS,I,C*,I,C*);
extern B        jtxlinit(JS);
extern B        jtxoinit(JS);
extern B        jtxsinit(JS);

extern B        all0(A);
extern B        all1(A);
extern I        allosize(A);
extern I        atype(I);
extern I        boxat(A,I,I,I);
extern I        bsum(I,B*);

extern CR       condrange(I *,I,I,I,I);
extern CR       condrange2(US *,I,I,I,I);
extern CR       condrange4(C4 *,I,I,I,I);
extern B        evoke(A);
struct fmtbuf   fmtlong(struct fmtbuf,E);
extern void     forcetomemory(void *);
extern void     freesymb(J,A);
extern void     freetstackallo(J);
extern UI       hic(I,UC*);
extern UI       hic2(I,UC*);
extern UI       hic4(I,UC*);
extern I        hsize(I);
extern J        jinit(void);
extern void     jsto(JS,I,C*);
extern void     jstpoll(J);
extern void     jststop(J);
extern I        level(J,A);
extern I        levelle(J,A,I);
extern void     mvc(I,void*,I,void*);
extern B        nameless(A);
extern I        plus1I2II AH2A(I,I,I2*,I*,I*,J);
extern I        plus1I4II AH2A(I,I,I4*,I*,I*,J);
extern void     protectlocals(J,I);
extern I        pppp(J,A,A);
extern D        qpf(void);
extern A        relocate(I,A);
extern I        remii(I,I);
extern C        spellin(I,C*);
extern void     spellit(C,C*);
extern I        smmallosize(A);
extern void     smmfrr(A);
#if ((MEMAUDIT&5)==5) && SY_64 // scaf
extern void testbuf(A);
#endif
extern I        sqrtE(J,I,E*,E*);
extern void     vvsortqs8ai(IL*,I);
extern void     vvsortqs8ao(IL*,IL*,I);
extern void     vvsortqs4ai(I4*,I);
extern void     vvsortqs4ao(I4*,I4*,I);
#if BW==64
#define vvsortqiai vvsortqs8ai
#define vvsortqiao vvsortqs8ao
#else
#define vvsortqiai vvsortqs4ai
#define vvsortqiao vvsortqs4ao
#endif
extern I        strtoI10s(I,C*);
extern D        tod(void);
extern void     va1primsetup(A);
extern void     va2primsetup(A);
extern B        vlocnm(I,C*);
#if PYXES
extern void     readlock(S*,S);
extern void     writelock(S*,S);
#endif
#if C_AVX2 || EMU_AVX2
extern I        memcmpne(void*, void*, I);
#else
#define memcmpne(s,t,l) (!!memcmp((s),(t),(l)))
#endif

struct __attribute__((aligned(CACHELINESIZE))) Bi1 {I hdr[AKXR(0)/SZI]; I v[1];};  // data is one integer atom
struct __attribute__((aligned(CACHELINESIZE))) Bd1 {I hdr[AKXR(0)/SZI]; D v[1];};  // data is one float atom
struct __attribute__((aligned(CACHELINESIZE))) Bd2 {I hdr[AKXR(1)/SZI]; D v[2];};  // data for the multi-word atom is aligned to cacheline
extern struct Bd2 Ba0j1;
#define a0j1 ((A)&Ba0j1)
extern struct Bd1 Bainf;
#define ainf ((A)&Bainf)
extern I Baqq[];
#define aqq ((A)&Baqq)
extern I Bmtv[];
#define mtv ((A)&Bmtv)
extern I Bmtvi[];
#define mtvi ((A)&Bmtvi)
extern I Bmtm[];
#define mtm ((A)&Bmtm)
extern I Bmtmi[];
#define mtmi ((A)&Bmtmi)
extern I Basgnlocsimp[];
#define asgnlocsimp ((A)&Basgnlocsimp)
extern I Basgngloname[];
#define asgngloname ((A)&Basgngloname)
extern I Basgnforceglo[];
#define asgnforceglo ((A)&Basgnforceglo)
extern I Basgnforcegloname[];
#define asgnforcegloname ((A)&Basgnforcegloname)
extern I Bimax[];
#define imax ((A)&Bimax)
extern I Bmark[];
#define mark ((A)&Bmark)
extern I Bchrcolon[];
#define chrcolon ((A)&Bchrcolon)
extern struct Bd1 Bmarkd[];
#define markd(n) ((A)&Bmarkd[n])  // QP,FL,CMPX
extern I Bchrspace[];
#define chrspace ((A)&Bchrspace)
#if !SY_64
extern long long validitymask[];
#else
extern I validitymask[];
extern I maskec4123[];
#endif
extern C  ctype[];
extern const double dzero;
extern D        inf;
extern D        infm;
extern I Biv0[];
#define iv0 ((A)&Biv0)
extern I Biv1[];
#define iv1 ((A)&Biv1)
extern A        mnuvxynam[6];
extern void     moveparseinfotosi(J);
extern I Bnum[][8*(2-SY_64)];
#define zeroionei(n) ((A)(Bnum+(n)))
#define num(n) ((A)(Bnum+2+(n)-NUMMIN))
#define I1mem (iotavec-IOTAVECBEGIN+1)  // 1 stored in memory
extern struct Bd1 Bnumvr[];
#define numvr(n) ((A)(Bnumvr+(n)))
extern struct Bi1 Bnumi2[];
#define numi2(n) ((A)(Bnumi2+(n)))
extern struct Bi1 Bnumi4[];
#define numi4(n) ((A)(Bnumi4+(n)))
extern PSTK initparserstack[4];
extern struct Bd1 Bonehalf;
#define onehalf ((A)&Bonehalf)
#define minus0 Bnumvr[0].v[0]  // float -0
extern struct Bd2 BpieE;
#define pieE ((A)&BpieE)
#define epi (*(E*)&BpieE.v)
extern D        pf;
extern struct Bd1 Bpie;
#define pie ((A)&Bpie)
extern PRIM dsCMINUS_NOT;  // -. that was converted to -
extern PRIM dsCMINUS_NEG;  // -. that was converted to -
extern PRIM dsCMINUS_DEC;  // -. that was converted to -
extern PRIM dsCPLUS_INC;  // >: that was converted to +
extern PRIM dsCSTAR_DUBL;  // +: that was converted to *
extern PRIM dsCSTAR_SQUARE;  // *: that was converted to *
extern PRIM dsCSTAR_HALVE;  // -: that was converted to *
extern PRIM dsCSTAR_CIRCLE;  // o. that was converted to *
extern PRIM dsCDIV_RECIP;  // % that was converted to %
extern PRIM dsCDIV_HALVE;  // -: that was converted to %
extern PRIM     primtab[];
extern Z        zeroZ;
extern const dcomplex zone;
extern const dcomplex zzero;
extern I oneone[2];
// extern A        zpath;
extern I iotavec[IOTAVECLEN];  // ascending integers, starting at IOTAVECBEGIN
/* cpu feature */
extern UC       hwaes;
extern UC       hwfma;
extern VARPSA rpsnull;
extern PRIM sfn0overself;
extern UA va1tab[];
extern VA va[];
#if C_AVX2 || EMU_AVX2
extern __m256d initecho(void *);
#endif
extern D two_52;  // 2^52
extern D two_84_63;  // 2^84+2^63
extern D two_84_63_52;  // 2^84 + 2^63 + 2^52  for int-float conversion
extern I disttosign[4];  // bit between (bit 0 of byte) and sign bit of lane
extern I Ivalidboolean;  // masks 
extern I Iimin;
extern I Iimax;
extern I4 charfill;
extern I fortesting;

#if CRASHLOG // debugging
extern int logfirsttime;
extern int logparm;  // set to control logging inside m.c
extern char logarea[200];  // where messages are built
extern void writetolog(J,C *);
#endif


#if (SYS & SYS_ATARIST+SYS_ATT3B1)
extern int      memcmp();       /* C library fn                            */
extern D        strtod();       /* C library fn                            */
extern I        strtol();       /* C library fn                            */
#endif
