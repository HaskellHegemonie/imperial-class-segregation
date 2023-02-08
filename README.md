### ccp-vaccum
### Class segregation written in C, but fast as fuck
### just make your day using (gnu) make(1)

Getting Started: 
```
make
./reg.out [FILE] [CLASS_FILE] [DIRECTORY(optional)] 
```
  
 Examples
```
./reg.out subst_000.htm class sibiria
will split the individual classes to siberia
```

Class files are in the following format (example found in ./class)
```
[1-2 digit number][letters] 
or
[letter][1-2 digit number]
so something like
7abcd
will produce files 
7a.htm
7b.htm
7c.htm
7d.htm
with just the header information if they don't have any sections

like how Q11
will produce Q11.htm with just the header if they have no section
```
