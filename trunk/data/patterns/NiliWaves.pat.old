PATTERNS:

    ###################################
    # Upward Impulsive Waves
    ###################################

    ImpulseClassicUp = up_1 down_2 up_3 down_4 up_5 @
        up_1    -> true,
        down_2  -> min(down_2) > min(up_1),
        up_3    -> value(up_3) > value(down_2),
        down_4  -> min(down_4) > min(up_3),
        up_5    -> value(up_5) > value(down_4)  and  
                  (value(up_3) > value(up_1)  or  value(up_3) > value(up_5));
                  
    ImpulseFailedFifthUp = up_1 down_2 up_3 down_4 up_5 @
        up_1    -> true,
        down_2  -> min(down_2) > min(up_1),
        up_3    -> value(up_3) > value(down_2),
        down_4  -> min(down_4) > min(up_3),
        up_5    -> value(up_5) < value(down_4)  and  value(up_5) >= 0.382*value(down_4) and
                   (value(up_3) > value(up_1)  or  value(up_3) > value(up_5));

    ###################################
    # Downcast Corrective Waves
    ###################################
    
    CorrectionFlatDn = down_1 up_2 down_3 @
        down_1 -> isprev(ImpulseUp),
        up_2   -> value(up_2) <= 1.382*value(down_1)  and
                  value(up_2) >= 0.618*value(down_1),
        down_3 -> value(down_3) <= 1.382*value(up_2)  and
                  value(down_3) >= 0.382*value(up_2);
                  
    CorrectionZigzagDn = down_1 up_2 down_3 @
        down_1 -> isprev(ImpulseUp),
        up_2   -> value(up_2) >= 0.382*value(down_1) and value(up_2) <= 0.618*value(down_1),
        down_3 -> min(down_3) < min(up_2);
    
    
    
    ###################################
    # Downcast Impulsive Waves
    ###################################
    ImpulseClassicDn = down_1 up_2 down_3 up_4 down_5 @
        down_1  -> true,
        up_2    -> max(up_2) < max(down_1),
        down_3  -> value(down_3) > value(up_2),
        up_4    -> max(up_4) < max(down_3),
        down_5  -> value(down_5) > value(up_4)  and  
                  (value(down_3) > value(down_1)  or  value(down_3) > value(down_5));
                  
    ImpulseFailedFifthDn = down_1 up_2 down_3 up_4 down_5 @
        down_1  -> true,
        up_2    -> max(up_2) < max(down_1),
        down_3  -> value(down_3) > value(up_2),
        up_4    -> max(up_4) < max(down_3),
        down_5  -> value(down_5) < value(up_4)  and  value(down_5) >= 0.382*value(up_4)  and  
                  (value(down_3) > value(down_1)  or  value(down_3) > value(down_5));
    
    
    ###################################
    # Upward Corrective Waves
    ###################################
    
    CorrectionFlatUp = up_1 down_2 up_3 @
        up_1   -> isprev(ImpulseDown),
        down_2 -> value(down_2) <= 1.382*value(up_1)  and
                  value(down_2) >= 0.618*value(up_1),
        up_3   -> value(up_3) <= 1.382*value(down_2)  and
                  value(up_3) >= 0.382*value(down_2);
    
    CorrectionZigzagDn = up_1 down_2 up_3 @
        up_1   -> isprev(ImpulseDown),
        down_2 -> value(down_2) >= 0.382*value(up_1) and value(down_2) <= 0.618*value(up_1),
        up_3   -> max(up_3) > max(down_2);
    
SYNONYMS:
    ImpulseUp = ImpulseClassicUp, ImpulseFailedFifthUp;
    ImpulseDown = ImpulseClassicDn, ImpulseFailedFifthDn;