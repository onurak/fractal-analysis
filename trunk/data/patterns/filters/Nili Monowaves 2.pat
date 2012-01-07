PATTERNS:
    up = a | up up | up_1 dn_2 up_3 @
        up_3: (length(dn_2) < length(up_1) * 0.4) and
              (length(dn_2) < length(up_3) * 0.4);
    dn = b | dn dn | dn_1 up_2 dn_3 @
        dn_3: ((length(up_2) < length(dn_1) * 0.4) and
              (length(up_2) < length(dn_3) * 0.4));