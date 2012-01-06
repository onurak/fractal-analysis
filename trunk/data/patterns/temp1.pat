PATTERNS:
    up = a_1 b_2 a_3 | a @
        a_3: length(b_2) < (length(a_1) + length(a_3))*0.5;
        
    dn = b_1 a_2 b_3 | b @
        b_3: length(a_2) < (length(b_1) + length(b_3))*0.5;
            