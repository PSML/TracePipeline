: 2DUP OVER OVER ;

: S0
  ( ADDRESS OF THE START OF THE DATA STACK )
  ( BY JAMES RENEAU - 2012-05-21 )
  (   - LICENSED UNDER CREATIVE COMMONS A-NC-SA )
  206
;

: PICK ( N -- N )
  ( NONDESTRRUCTIVE PEEK ON THE DATA STACK )
  ( FROM TOP - 0 .. STACKHEIGHT-1 )
  ( BY JAMES RENEAU - 2012-05-21 )
  (   - LICENSED UNDER CREATIVE COMMONS A-NC-SA )
  DUP 0 < IF
    2 * S0 2 + + @
  ELSE
    1 + 2 * SP@ SWAP + @
  ENDIF
;

: 2DROP ( D1 -- )
  ( DROP DOUBE WORD )
  ( N1 N2 -- )
  ( BY JAMES RENEAU - 2012-05-21 )
  (   - LICENSED UNDER CREATIVE COMMONS A-NC-SA )
  DROP DROP
;
: 2OVER ( D1 D2 -- D1 D2 D1 )
  ( DOUBLE WORD OVER )
  ( N1 N2 N3 N4 -- N2 N1 N4 N3 N2 N1 )
  ( BY JAMES RENEAU - 2012-05-21 )
  (   - LICENSED UNDER CREATIVE COMMONS A-NC-SA )
  3 PICK
  3 PICK
;

: 2SWAP ( D1 D2 -- D1 D2 )
  ( SWAP DOUBLE WORDS )
  ( N1 N2 N3 N4 -- N1 N2 N3 N4 )
  ( BY JAMES RENEAU - 2012-05-21 )
  (   - LICENSED UNDER CREATIVE COMMONS A-NC-SA )
  2 PICK >R
  3 PICK >R
  0 PICK >R
  1 PICK >R
  2DROP 2DROP
  R> R> R> R>
;

: FIB 
  DROP   ( DL,DH -- DL )
  DUP    ( DL -- DL DL ) 
  IF      ( DL DL - DL : if input is not zero )
    0. ROT ( DL -- 0,0 DL )
    1. ROT ( 0,0 DL -- 0,0 1,0 DL )
    0      ( 0,0 1,0 DL -- 0,0 1,0 DL 0 )
    DO     ( 0,0 1,0 DL 0 -- 0,0 1,0 )
      2OVER ( 0,0 1,0 --  0,0 1,0 0,0 )  
      D+
      2SWAP
    LOOP
    2DROP
  ELSE   ( input was zero ) 
    0
  THEN ;

: FIBLOOP
  ( Driver for Fib calculation )
  CR                          ( newline : )
  HERE                        ( start of free space as buf : buf )
  80 ALLOT                    ( allocate 80 bytes as a buffer : buf )
  BEGIN                       ( main loop : buf ) 
   DUP                        ( : buf buf )
   80 EXPECT                  ( read upto 80 characters : buf )

   ( next line was tricky to figure out )
   ( could not get plain NUMBER to work )
     ( push 32 bit zero         : buf 0,0 )
     ( push buf -1              : buf 0,0 buf-1 )
     ( invoke number )
   0. HERE 81 - (NUMBER)      ( : buf dl,dh addr )
                              ( if input was not a postive number)
                              ( buf and addr will be equal )
   HERE 80 -                  ( : buf dl,dh addr buf )
   =                          ( ? addr=buf -> t else f : buf dl,dh t|f )
   IF MON ENDIF               ( if no number was found exit via mon)
 
   OVER                       ( : buf dl,dh dl )
   47 >                       ( if dl > 47 : buf dl,dh )
   IF                        
    ." -1 "                   ( print -1 )
    DROP DROP                 ( : buf )
   ELSE
     FIB               ( call fib : buf  dl,dh )
     <# #S #> TYPE     ( print as unsigned )
   ENDIF
   CR                         ( newline )
AGAIN ;                       ( got to top of loop )
FIBLOOP
0
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
26
27
28
29
30
31
32
33
34
35
36
37
38
39
40
41
42
43
44
45
46
47
48
X
