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
MON
