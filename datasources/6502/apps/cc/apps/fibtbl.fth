FIBTBL 0 + @ FIBTBL 2 + @ <# #S #> TYPE SPACE
FIBTBL 4 + @ FIBTBL 6 + @ <# #S #> TYPE SPACE
FIBTBL 8 + @ FIBTBL 10 + @ <# #S #> TYPE SPACE
FIBTBL 12 + @ FIBTBL 14 + @ <# #S #> TYPE SPACE
FIBTBL 16 + @ FIBTBL 18 + @ <# #S #> TYPE SPACE
FIBTBL 20 + @ FIBTBL 22 + @ <# #S #> TYPE SPACE
FIBTBL 24 + @ FIBTBL 26 + @ <# #S #> TYPE SPACE
FIBTBL 28 + @ FIBTBL 30 + @ <# #S #> TYPE SPACE
FIBTBL 32 + @ FIBTBL 34 + @ <# #S #> TYPE SPACE
FIBTBL 36 + @ FIBTBL 38 + @ <# #S #> TYPE SPACE
FIBTBL 40 + @ FIBTBL 42 + @ <# #S #> TYPE SPACE
FIBTBL 44 + @ FIBTBL 46 + @ <# #S #> TYPE SPACE
FIBTBL 48 + @ FIBTBL 50 + @ <# #S #> TYPE SPACE
FIBTBL 52 + @ FIBTBL 54 + @ <# #S #> TYPE SPACE
FIBTBL 56 + @ FIBTBL 58 + @ <# #S #> TYPE SPACE
FIBTBL 60 + @ FIBTBL 62 + @ <# #S #> TYPE SPACE
FIBTBL 64 + @ FIBTBL 66 + @ <# #S #> TYPE SPACE
FIBTBL 68 + @ FIBTBL 70 + @ <# #S #> TYPE SPACE
FIBTBL 72 + @ FIBTBL 74 + @ <# #S #> TYPE SPACE
FIBTBL 76 + @ FIBTBL 78 + @ <# #S #> TYPE SPACE
FIBTBL 80 + @ FIBTBL 82 + @ <# #S #> TYPE SPACE
FIBTBL 84 + @ FIBTBL 86 + @ <# #S #> TYPE SPACE
FIBTBL 88 + @ FIBTBL 90 + @ <# #S #> TYPE SPACE
FIBTBL 92 + @ FIBTBL 94 + @ <# #S #> TYPE SPACE
FIBTBL 96 + @ FIBTBL 98 + @ <# #S #> TYPE SPACE
FIBTBL 100 + @ FIBTBL 102 + @ <# #S #> TYPE SPACE
FIBTBL 104 + @ FIBTBL 106 + @ <# #S #> TYPE SPACE
FIBTBL 108 + @ FIBTBL 110 + @ <# #S #> TYPE SPACE
FIBTBL 112 + @ FIBTBL 114 + @ <# #S #> TYPE SPACE
FIBTBL 116 + @ FIBTBL 118 + @ <# #S #> TYPE SPACE
FIBTBL 120 + @ FIBTBL 122 + @ <# #S #> TYPE SPACE
FIBTBL 124 + @ FIBTBL 126 + @ <# #S #> TYPE SPACE
FIBTBL 128 + @ FIBTBL 130 + @ <# #S #> TYPE SPACE
FIBTBL 132 + @ FIBTBL 134 + @ <# #S #> TYPE SPACE
FIBTBL 136 + @ FIBTBL 138 + @ <# #S #> TYPE SPACE
FIBTBL 140 + @ FIBTBL 142 + @ <# #S #> TYPE SPACE
FIBTBL 144 + @ FIBTBL 146 + @ <# #S #> TYPE SPACE
FIBTBL 148 + @ FIBTBL 150 + @ <# #S #> TYPE SPACE
FIBTBL 152 + @ FIBTBL 154 + @ <# #S #> TYPE SPACE
FIBTBL 156 + @ FIBTBL 158 + @ <# #S #> TYPE SPACE
FIBTBL 160 + @ FIBTBL 162 + @ <# #S #> TYPE SPACE
FIBTBL 164 + @ FIBTBL 166 + @ <# #S #> TYPE SPACE
FIBTBL 168 + @ FIBTBL 170 + @ <# #S #> TYPE SPACE
FIBTBL 172 + @ FIBTBL 174 + @ <# #S #> TYPE SPACE
FIBTBL 176 + @ FIBTBL 178 + @ <# #S #> TYPE SPACE
FIBTBL 180 + @ FIBTBL 182 + @ <# #S #> TYPE SPACE
FIBTBL 184 + @ FIBTBL 186 + @ <# #S #> TYPE SPACE
FIBTBL 188 + @ FIBTBL 190 + @ <# #S #> TYPE SPACE
: FIB         ( dl,dh )
    DROP 
    4 *           ( dl -- 4dl ) 
    FIBTBL +      ( 4dl -- FIBTBL+4dl )
    DUP           ( FIBTBL+4dl -- FIBTBL+4dl FIBTBL+4dl )
    @             ( FIBTBL+4dl FIBTBL+4dl  -- FIBTBL+4dl FIBTBL[4dl] )
    SWAP          ( FIBTBL+4dl FIBTBL[4dl] -- FIBTBL[4dl] FIBTBL+4dl )
    2 +           ( FIBTBL[4dl] FIBTBL+4dl -- FIBTBL[4dl] FIBTBL+4dl+2 )
    @             ( FIBTBL[4dl] FIBTBL+4dl+2 -- FIBTBL[4dl] FIBTBL[4dl+2] )
;
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
X
