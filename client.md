```
>>reset 
Do you want to reset the database?[y/n]
y
>>size db
2
>>create map shiyan s s
>>size map shiyan
0
>>add map shiyan apple pen
>>add map shiyan pine apple
>>add map shiyan william king5
>>show map shiyan
[0]	"apple"	:	"pen"
[1]	"pine"	:	"apple"
[2]	"william"	:	"king5"
>>size map shiyan
3
>>create map shiyan1
>>create map shiyan2
>>size db
11
>>drop shiyan1
>>drop map shiyan1
>>drop map shiyan2
>>drop map shiyan
>>size db
2
>>create map shiyan
>>add map shiyan abc def
>>show map shiyan
[0]	"abc"	:	"def"
>>update map shiyan abc zzz
>>show map shiyan   
[0]	"abc"	:	"zzz"
>>reset              
Do you want to reset the database?[y/n]
y
>>create list shiyan
>>add list shiyan aaa
>>add list shiyan bbb
>>show list shiyan 0 100
[0]	:	"aaa"
[1]	:	"bbb"
>>update list shiyan 1 ccc
>>show list shiyan 0 100
[0]	:	"aaa"
[1]	:	"ccc"
>>drop list shiyan
>>create map shiyan
>>add map shiyan www bbb
>>add map shiyan ccc zzz
>>show map shiyan
[0]	"ccc"	:	"zzz"
[1]	"www"	:	"bbb"
>>delete shiyan ccc
>>show map shiyan
[0]	"www"	:	"bbb"
>>exit
Good Bye~
```
