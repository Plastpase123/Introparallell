# Document Title

integral():
    

main():
    number of threads = no_threads
    number of traps = w
    
    definera lista med punkter som ska evalueras
    list = []
    for i in w:
        num = 1/i
        list.append(num)
    
    
    (f(x_j-1)-f(x)*(1/w))/2 + f(x)*(1/w)
    
    
    
    rest = w mod threads
    
    t1 [0, w + rest]
   
   
   18 jobb
   5 threads
   steg = 18 - 18 % 5 / 5 = 3
   
   
   t1: start 0, slut vid 5
   
   t2: start 6, slut 8
   
   t3: start 9, slut 11
   
   t4: start 12, slut 14
   
   t5: start 15, slut 17
