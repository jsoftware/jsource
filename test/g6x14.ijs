prolog './g6x14.ijs'

NB. epoch datetime is nanoseconds from 2000-01-01
NB. foreigns convert between epochdt and iso 8601 extended format

IMIN=: _9223372036854775808
sfe=: 6!:16
efs=: 6!:17
efp=: 6!:18

empty 0 : 0
// apple freebsd openbsd
#define LC_ALL          0
#define LC_COLLATE      1
#define LC_CTYPE        2
#define LC_MONETARY     3
#define LC_NUMERIC      4
#define LC_TIME         5
#define LC_MESSAGES     6

/* Bit shifting order of LC_*_MASK should match XLC_* and LC_* order. */
#define LC_COLLATE_MASK  (1<<0)
#define LC_CTYPE_MASK    (1<<1)
#define LC_MONETARY_MASK (1<<2)
#define LC_NUMERIC_MASK  (1<<3)
#define LC_TIME_MASK     (1<<4)
#define LC_MESSAGES_MASK (1<<5)
#define LC_ALL_MASK      (LC_COLLATE_MASK | LC_CTYPE_MASK | LC_MESSAGES_MASK | \
                          LC_MONETARY_MASK | LC_NUMERIC_MASK | LC_TIME_MASK)
#define LC_VERSION_MASK  (1<<6)
#define LC_GLOBAL_LOCALE ((locale_t)-1)

// linux
#define LC_CTYPE		 0
#define LC_NUMERIC		 1
#define LC_TIME		 2
#define LC_COLLATE		 3
#define LC_MONETARY		 4
#define LC_MESSAGES		 5
#define LC_ALL		 6

// windows
#define LC_ALL          0
#define LC_COLLATE      1
#define LC_CTYPE        2
#define LC_MONETARY     3
#define LC_NUMERIC      4
#define LC_TIME         5
)

loc=: >@{. ((unxlib 'c'),' duplocale x x')&(15!:0) ^:(UNAME-:'Linux') <_1
loc=: >@{. ((unxlib 'c'),' newlocale x i *c x')&(15!:0) ^:(UNAME-:'Linux') 4;(,'C');loc   NB. LC_TIME_MASK  1 << 2
empty ((unxlib 'c'),' uselocale x x')&(15!:0) ^:(UNAME-:'Linux') <loc
loc=: >@{. ((unxlib 'c'),' duplocale x x')&(15!:0) ^:(IFIOS<(<UNAME) e.;:'Darwin FreeBSD OpenBSD') <_1
loc=: >@{. ((unxlib 'c'),' newlocale x i *c x')&(15!:0) ^:(IFIOS<(<UNAME) e.;:'Darwin FreeBSD OpenBSD') 32;(,'C');loc   NB. LC_TIME_MASK  5 << 2
empty ((unxlib 'c'),' uselocale x x')&(15!:0) ^:(IFIOS<(<UNAME) e.;:'Darwin FreeBSD OpenBSD') <loc

0 = efs '2000-01-01'                   NB. epochdt 0 is 2000-01-01
3313526400000000000 = efs '2105'       NB. nanoseconds from 2000 to 2105
_946684800000000000 = efs '1970-01-01' NB. negative for nanoseconds before 2000
IMIN = efs '1700'    NB. before 1800 is invalid
IMIN = efs '2300'    NB. after  2200 is invalid
IMIN = efs '????'    NB. canonical invalid date rather than error
IMIN = efs '2000-00-01'
IMIN = efs '2000-13-01'
IMIN = efs '2000-01-00'
IMIN = efs '2000-01-32'
IMIN = efs '2000-01-01T25:00:00'
IMIN = efs '2000-01-01T01:60:00'
IMIN = efs '2000-01-01T01:00:61'
IMIN = efs '2000-01-01T01:00:00+24:00'
IMIN = efs '2000-01-01T01:00:00+00:60'
IMIN = efs '2000-01-01T01:00:00-24:00'
IMIN = efs '2000-01-01T01:00:00-00:60'

'1970-01-01' -: ', d' sfe efs '1970'
'1970-01-01T00:00:00' -: ', 0' sfe efs '1970'
'1970-01-01T00:00:00,000' -: ', 3' sfe efs '1970'
'1970-01-01T00:00:00,000000' -: ', 6' sfe efs '1970'
'1970-01-01T00:00:00,000000000' -: ', 9' sfe efs '1970'

'1970-01-01Z' -: ',Zd' sfe efs '1970'
'1970-01-01T00:00:00Z' -: ',Z0' sfe efs '1970'
'1970-01-01T00:00:00,000Z' -: ',Z3' sfe efs '1970'
'1970-01-01T00:00:00,000000Z' -: ',Z6' sfe efs '1970'
'1970-01-01T00:00:00,000000000Z' -: ',Z9' sfe efs '1970'

NB. iso 8601 examples
NB. -T:.,+- decorators required and validated
NB. . or , separates seconds from nanoseconds
a=: ><;._2 [ 0 : 0
2014-10-11T12:13:14.123456789+05:30
2014-10-11T12:13:14,123456789+05
2014-10-11T12:13:14.123456789
2014-10-11T12:13:14.123456789
2014-10-11T12:13:14.123
2014-10-11T12:13:14
2014-10-11T12:13
2014-10-11T12:13
2014-10-11T12+05
2014-10-11T12
2014-10-11
2014
)

e=: efs a

e = ".;._2 (0 : 0)
466324994123456789
466326794123456789
466344794123456789
466344794123456789
466344794123000000
466344794000000000
466344780000000000
466344780000000000
466326000000000000
466344000000000000
466300800000000000
441849600000000000
)

('. 9'sfe e) -: ];._2 (0 : 0) NB. nano
2014-10-11T06:43:14.123456789
2014-10-11T07:13:14.123456789
2014-10-11T12:13:14.123456789
2014-10-11T12:13:14.123456789
2014-10-11T12:13:14.123000000
2014-10-11T12:13:14.000000000
2014-10-11T12:13:00.000000000
2014-10-11T12:13:00.000000000
2014-10-11T07:00:00.000000000
2014-10-11T12:00:00.000000000
2014-10-11T00:00:00.000000000
2014-01-01T00:00:00.000000000
)

('. 3'sfe e) -: ];._2 (0 : 0) NB. milli
2014-10-11T06:43:14.123
2014-10-11T07:13:14.123
2014-10-11T12:13:14.123
2014-10-11T12:13:14.123
2014-10-11T12:13:14.123
2014-10-11T12:13:14.000
2014-10-11T12:13:00.000
2014-10-11T12:13:00.000
2014-10-11T07:00:00.000
2014-10-11T12:00:00.000
2014-10-11T00:00:00.000
2014-01-01T00:00:00.000
)

'2015-01-21' -: ', d' sfe efs '2015-01-22T00:00:00+02:00' NB. 2 hours subtracted to give utc

'2015-01-22' -: ', d' sfe efs '2015-01-22T00:00:00-02:00' NB. 2 hours added to give utc

NB. verify some edge conditions
t=: efs '1800-01-01'
'1800-01-01T00:00:00,000000000'-:', 9'sfe t
'?'={. ', 9'sfe t-1
'1800-01-01T00:00:00,000000001'-: ', 9'sfe t+1

t=: efs '2200-12-31T23:59:59,999999999'
'2200-12-31T23:59:59,999999999'-:', 9'sfe t
'2200-12-31T23:59:59,999999998'-:', 9'sfe t-1
'?'={.', 9'sfe t+1

NB. T or blank allowed as delimiter of time fields
(efs'2015-01-22T00:00:00+02:00')=efs'2015-01-22 00:00:00+02:00'
(efs'2015-01-22  ')=efs'2015-01-22'

IMIN = efs''

NB. monad
466300800000000000 = efs '2014-10-11'
466344794000000000 = efs '2014-10-11T12:13:14'
466344794123000000 = efs '2014-10-11T12:13:14,123'
466344794123456000 = efs '2014-10-11T12:13:14,123456'
466344794123456789 = efs '2014-10-11T12:13:14,123456789'
466344794123456789 = efs '2014-10-11T12:13:14,1234567890'   NB. extra precision
466344794123456789 = efs '2014-10-11T12:13:14,1234567899'
1:@:efs '1999-31T12:45:88.33193356'   NB. GIGO but no error

NB. dyad
466344794123456789 = '9' efs '2014-10-11T12:13:14,123456789'
466344794123000000 = '3' efs '2014-10-11T12:13:14,123456789'
466344794000000000 = '0' efs '2014-10-11T12:13:14,123456789'
466300800000000000 = 'd' efs '2014-10-11T12:13:14,123456789'
'nonce error' -: '6' efs etx '2014-10-11T12:13:14,123456789'
'domain error' -: 'a' efs etx '2014-10-11T12:13:14,123456789'

'1970-10-11T12:13:14,123456789' -: (', 9'&sfe)@:efs '1970-10-11T12:13:14,123456789'
'1970-10-11T12:13:14,123' -: (', 3'&sfe)@:efs '1970-10-11T12:13:14,123456789'
'1970-10-11T12:13:14' -: (', 0'&sfe)@:efs '1970-10-11T12:13:14,123456789'
'1970-10-11' -: (', d'&sfe)@:efs '1970-10-11T12:13:14,123456789'

'2014-10-11T12:13:14,123456789' -: (', 9'&sfe)@:efs '2014-10-11T12:13:14,123456789'
'2014-10-11T12:13:14,123' -: (', 3'&sfe)@:efs '2014-10-11T12:13:14,123456789'
'2014-10-11T12:13:14' -: (', 0'&sfe)@:efs '2014-10-11T12:13:14,123456789'
'2014-10-11' -: (', d'&sfe)@:efs '2014-10-11T12:13:14,123456789'

NB. rank
(2 10$'1970-10-11') -: ', d' sfe 2#_922189605876543211

NB. parse datetime string

'2022-12-20T14:30:18' -: sfe@:efp '2022-12-20T14:30:18'
'2022-12-20T14:30:18' -: sfe@:efp '2022-12-20T02:30:18 PM'
'2022-12-20T14:30:18' -: sfe@:efp '2022-12-20 14:30:18'
'2022-12-20T14:30:18' -: sfe@:efp '2022-12-20 02:30:18 PM'
'2022-12-20T00:00:00' -: sfe@:efp '2022-12-20'
'2022-12-20T14:30:18' -: sfe@:efp '12/20/2022 14:30:18'
'2022-12-20T14:30:18' -: sfe@:efp '12/20/2022 02:30:18 PM'
'2022-12-20T00:00:00' -: sfe@:efp '12/20/2022'
'2022-12-20T14:30:18' -: sfe@:efp '20/12/2022 14:30:18'
'2022-12-20T14:30:18' -: sfe@:efp '20/12/2022 02:30:18 PM'
'2022-12-20T00:00:00' -: sfe@:efp '20/12/2022'
'2022-12-20T14:30:18' -: sfe@:efp '20/Dec/2022 14:30:18'
'2022-12-20T14:30:18' -: sfe@:efp '20/Dec/2022 02:30:18 PM'
'2022-12-20T00:00:00' -: sfe@:efp '20/Dec/2022'
'2022-12-20T14:30:18' -: sfe@:efp 'Dec/20/2022 14:30:18'
'2022-12-20T14:30:18' -: sfe@:efp 'Dec/20/2022 02:30:18 PM'
'2022-12-20T00:00:00' -: sfe@:efp 'Dec/20/2022'
'2022-12-20T14:30:18' -: sfe@:efp '20 Dec 2022 14:30:18'
'2022-12-20T14:30:18' -: sfe@:efp '20 Dec 2022 02:30:18 PM'
'2022-12-20T00:00:00' -: sfe@:efp '20 Dec 2022'
'2022-12-20T14:30:18' -: sfe@:efp 'Dec 20 2022 14:30:18'
'2022-12-20T14:30:18' -: sfe@:efp 'Dec 20 2022 02:30:18 PM'
'2022-12-20T00:00:00' -: sfe@:efp 'Dec 20 2022'

NB. fractional second only applicable to format ended with %S

'2022-12-20T14:30:18.123456789' -: ('. 9'&sfe)@:('9'&efp) '2022-12-20T14:30:18.123456789'
'2022-12-20T14:30:18.123456789' -: ('. 9'&sfe)@:('9'&efp) '2022-12-20 14:30:18.123456789'
'2022-12-20T14:30:18.123456789' -: ('. 9'&sfe)@:('9'&efp) '12/20/2022 14:30:18.123456789'
'2022-12-20T14:30:18.123456789' -: ('. 9'&sfe)@:('9'&efp) '20/12/2022 14:30:18.123456789'
'2022-12-20T14:30:18.123456789' -: ('. 9'&sfe)@:('9'&efp) '20/Dec/2022 14:30:18.123456789'
'2022-12-20T14:30:18.123456789' -: ('. 9'&sfe)@:('9'&efp) 'Dec/20/2022 14:30:18.123456789'
'2022-12-20T14:30:18.123456789' -: ('. 9'&sfe)@:('9'&efp) '20 Dec 2022 14:30:18.123456789'
'2022-12-20T14:30:18.123456789' -: ('. 9'&sfe)@:('9'&efp) 'Dec 20 2022 14:30:18.123456789'

'2022-12-20T14:30:18.123000000' -: ('. 9'&sfe)@:('3'&efp) '2022-12-20T14:30:18.123456789'
'2022-12-20T14:30:18.123000000' -: ('. 9'&sfe)@:('3'&efp) '2022-12-20 14:30:18.123456789'
'2022-12-20T14:30:18.123000000' -: ('. 9'&sfe)@:('3'&efp) '12/20/2022 14:30:18.123456789'
'2022-12-20T14:30:18.123000000' -: ('. 9'&sfe)@:('3'&efp) '20/12/2022 14:30:18.123456789'
'2022-12-20T14:30:18.123000000' -: ('. 9'&sfe)@:('3'&efp) '20/Dec/2022 14:30:18.123456789'
'2022-12-20T14:30:18.123000000' -: ('. 9'&sfe)@:('3'&efp) 'Dec/20/2022 14:30:18.123456789'
'2022-12-20T14:30:18.123000000' -: ('. 9'&sfe)@:('3'&efp) '20 Dec 2022 14:30:18.123456789'
'2022-12-20T14:30:18.123000000' -: ('. 9'&sfe)@:('3'&efp) 'Dec 20 2022 14:30:18.123456789'

'2022-12-20T14:30:18.000000000' -: ('. 9'&sfe)@:('0'&efp) '2022-12-20T14:30:18.123456789'
'2022-12-20T14:30:18.000000000' -: ('. 9'&sfe)@:('0'&efp) '2022-12-20 14:30:18.123456789'
'2022-12-20T14:30:18.000000000' -: ('. 9'&sfe)@:('0'&efp) '12/20/2022 14:30:18.123456789'
'2022-12-20T14:30:18.000000000' -: ('. 9'&sfe)@:('0'&efp) '20/12/2022 14:30:18.123456789'
'2022-12-20T14:30:18.000000000' -: ('. 9'&sfe)@:('0'&efp) '20/Dec/2022 14:30:18.123456789'
'2022-12-20T14:30:18.000000000' -: ('. 9'&sfe)@:('0'&efp) 'Dec/20/2022 14:30:18.123456789'
'2022-12-20T14:30:18.000000000' -: ('. 9'&sfe)@:('0'&efp) '20 Dec 2022 14:30:18.123456789'
'2022-12-20T14:30:18.000000000' -: ('. 9'&sfe)@:('0'&efp) 'Dec 20 2022 14:30:18.123456789'

'2022-12-20T00:00:00.000000000' -: ('. 9'&sfe)@:('d'&efp) '2022-12-20T14:30:18.123456789'
'2022-12-20T00:00:00.000000000' -: ('. 9'&sfe)@:('d'&efp) '2022-12-20 14:30:18.123456789'
'2022-12-20T00:00:00.000000000' -: ('. 9'&sfe)@:('d'&efp) '12/20/2022 14:30:18.123456789'
'2022-12-20T00:00:00.000000000' -: ('. 9'&sfe)@:('d'&efp) '20/12/2022 14:30:18.123456789'
'2022-12-20T00:00:00.000000000' -: ('. 9'&sfe)@:('d'&efp) '20/Dec/2022 14:30:18.123456789'
'2022-12-20T00:00:00.000000000' -: ('. 9'&sfe)@:('d'&efp) 'Dec/20/2022 14:30:18.123456789'
'2022-12-20T00:00:00.000000000' -: ('. 9'&sfe)@:('d'&efp) '20 Dec 2022 14:30:18.123456789'
'2022-12-20T00:00:00.000000000' -: ('. 9'&sfe)@:('d'&efp) 'Dec 20 2022 14:30:18.123456789'

NB. tzsec parameter
0 = (('0';3600) efs '2022-12-20T00:00:00') - (efs '2022-12-20T00:00:00')
3600000000000 = (('0';3600) efs '2022-12-20T00:00:00Z') - (efs '2022-12-20T00:00:00Z')
3600000000000 = (('0';3600) efs '2022-12-20T00:00:00+06') - (efs '2022-12-20T00:00:00+06')
'domain error' -: (('0';3600000) efs etx '2022-12-20T00:00:00+06')

NB. parse datetime string

loc=: >@{. ((unxlib 'c'),' duplocale x x')&(15!:0) ^:(IFIOS<(<UNAME) e.;:'Darwin FreeBSD OpenBSD') <_1
loc=: >@{. ((unxlib 'c'),' newlocale x i *c x')&(15!:0) ^:(IFIOS<(<UNAME) e.;:'Darwin FreeBSD OpenBSD') 32;(,'fr_FR.UTF-8');loc
empty ((unxlib 'c'),' uselocale x x')&(15!:0) ^:(IFIOS<(<UNAME) e.;:'Darwin FreeBSD OpenBSD') <loc

(IFIOS+.(<UNAME)-.@e.;:'Darwin') +. '2022-12-20T14:30:18' -: sfe@:efp '20 déc 2022 14:30:18'
(IFIOS+.(<UNAME)-.@e.;:'Darwin') +. '2022-12-20T14:30:18' -: sfe@:efp '2022-12-20T14:30:18'
(IFIOS+.(<UNAME)-.@e.;:'Darwin') +. '2022-12-20T02:30:18' -: sfe@:efp '2022-12-20T02:30:18 PM'
(IFIOS+.(<UNAME)-.@e.;:'Darwin') +. '2022-12-20T14:30:18' -: sfe@:efp '2022-12-20 14:30:18'
(IFIOS+.(<UNAME)-.@e.;:'Darwin') +. '2022-12-20T02:30:18' -: sfe@:efp '2022-12-20 02:30:18 PM'
(IFIOS+.(<UNAME)-.@e.;:'Darwin') +. '2022-12-20T00:00:00' -: sfe@:efp '2022-12-20'
(IFIOS+.(<UNAME)-.@e.;:'Darwin') +. '2022-12-20T14:30:18' -: sfe@:efp '12/20/2022 14:30:18'
(IFIOS+.(<UNAME)-.@e.;:'Darwin') +. '2022-12-20T02:30:18' -: sfe@:efp '12/20/2022 02:30:18 PM'
(IFIOS+.(<UNAME)-.@e.;:'Darwin') +. '2022-12-20T00:00:00' -: sfe@:efp '12/20/2022'
(IFIOS+.(<UNAME)-.@e.;:'Darwin') +. '2022-12-20T14:30:18' -: sfe@:efp '20/12/2022 14:30:18'
(IFIOS+.(<UNAME)-.@e.;:'Darwin') +. '2022-12-20T02:30:18' -: sfe@:efp '20/12/2022 02:30:18 PM'
(IFIOS+.(<UNAME)-.@e.;:'Darwin') +. '2022-12-20T00:00:00' -: sfe@:efp '20/12/2022'
(IFIOS+.(<UNAME)-.@e.;:'Darwin') +. '2022-12-20T14:30:18' -: sfe@:efp '20/Déc/2022 14:30:18'
(IFIOS+.(<UNAME)-.@e.;:'Darwin') +. '2022-12-20T02:30:18' -: sfe@:efp '20/Déc/2022 02:30:18 PM'
(IFIOS+.(<UNAME)-.@e.;:'Darwin') +. '2022-12-20T00:00:00' -: sfe@:efp '20/Déc/2022'
(IFIOS+.(<UNAME)-.@e.;:'Darwin') +. '2022-12-20T14:30:18' -: sfe@:efp 'Déc/20/2022 14:30:18'
(IFIOS+.(<UNAME)-.@e.;:'Darwin') +. '2022-12-20T02:30:18' -: sfe@:efp 'Déc/20/2022 02:30:18 PM'
(IFIOS+.(<UNAME)-.@e.;:'Darwin') +. '2022-12-20T00:00:00' -: sfe@:efp 'Déc/20/2022'
(IFIOS+.(<UNAME)-.@e.;:'Darwin') +. '2022-12-20T14:30:18' -: sfe@:efp '20 Déc 2022 14:30:18'
(IFIOS+.(<UNAME)-.@e.;:'Darwin') +. '2022-12-20T02:30:18' -: sfe@:efp '20 Déc 2022 02:30:18 PM'
(IFIOS+.(<UNAME)-.@e.;:'Darwin') +. '2022-12-20T00:00:00' -: sfe@:efp '20 Déc 2022'
(IFIOS+.(<UNAME)-.@e.;:'Darwin') +. '2022-12-20T14:30:18' -: sfe@:efp 'Déc 20 2022 14:30:18'
(IFIOS+.(<UNAME)-.@e.;:'Darwin') +. '2022-12-20T02:30:18' -: sfe@:efp 'Déc 20 2022 02:30:18 PM'
(IFIOS+.(<UNAME)-.@e.;:'Darwin') +. '2022-12-20T00:00:00' -: sfe@:efp 'Déc 20 2022'

loc=: >@{. ((unxlib 'c'),' duplocale x x')&(15!:0) ^:(IFIOS<(<UNAME) e.;:'Darwin FreeBSD OpenBSD') <_1
loc=: >@{. ((unxlib 'c'),' newlocale x i *c x')&(15!:0) ^:(IFIOS<(<UNAME) e.;:'Darwin FreeBSD OpenBSD') 32;(,'C');loc
empty ((unxlib 'c'),' uselocale x x')&(15!:0) ^:(IFIOS<(<UNAME) e.;:'Darwin FreeBSD OpenBSD') <loc


epilog''

