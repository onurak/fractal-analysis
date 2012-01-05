PATTERNS:
    L = up_1  ?_2  up_3 | L dn G @
        ?_2:  value(node()) < value(up_1);

	G = up_1  ?_2  up  |  (L_1 | G_1)  ?_2  (L | G) @
        ?_2:  value(node()) > value(?_1);
        
    
    