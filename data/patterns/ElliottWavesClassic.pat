PATTERNS:
    
    ImpulseUP = 
        up_1 dn_2 up_3 dn_4 up_5  |
        ImpulseUP_1  ZigZagSDN_2  ImpulseUP_3  ZigZagSDN_2  ImpulseUP_5
        @
        dn_2: min(dn_2) > min(up_1),
        up_3: max(up_3) >= max(up_1),
        dn_4: min(dn_4) >= max(up_1),
        up_5: (value(up_3) >= value(up_1) or value(up_3) >= value(up_5))
              and
              (max(up_5) >= max(up_3));
              

    ImpulseDN = dn_1 up_2 dn_3 up_4 dn_5 @
        up_2: max(up_2) < max(dn_1),
        dn_3: min(dn_3) <= min(dn_1),
        up_4: max(up_4) <= min(dn_1),
        dn_5: (value(dn_3) >= value(dn_1) or value(dn_3) >= value(dn_5))
              and
              (min(dn_5) <= min(dn_3));

              
              
              
    ZigZagSDN = 
        dn_1 up_2 dn_3  |
        ImpulseDN_1  ZigZagSUP_2  ImpulseDN_3
        @
        ?_1 : isprev(ImpulseUP),
        up_2: max(up_2) < max(dn_1),
        dn_3: (min(dn_3) < min(up_2))
              and
              (min(self()) < max(prev()));
              
    ZigZagSUP = up_1 dn_2 up_3 @ 
        up_1: isprev(ImpulseDN),
        dn_2: min(dn_2) > min(up_1),
        up_3: (max(up_3) > max(dn_2))
              and
              (max(self()) > min(prev()));
              
