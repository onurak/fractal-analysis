PATTERNS:
	imup = up_1 dn_2 up_3 dn_4 up_5 | imup crdn imup crdn imup @ 
		dn_2: min(dn_2) > min(up_1),
        up_3: max(up_3) > max(up_1),
        dn_4: min(dn_4) > max(up_1),
        up_5: max(up_5) > max(up_3);

    crdn = dn_1 up_2 dn_3 | imdn_1 crup_2 imdn_3 @
        dn_1: prev(imup),
        dn_3: max(prev()) > min(dn_3),
        imdn_1: prev(imup),
        imdn_3: max(prev()) > min(imdn_3);
    
    
    imdn = dn_1 up_2 dn_3 up_4 dn_5 | imdn crup imdn crup imdn @ 
		up_2: max(up_2) < max(dn_1),
        dn_3: min(dn_3) < min(dn_1),
        up_4: max(up_4) < min(dn_1),
        dn_5: min(dn_5) < min(dn_3);

    crup = up_1 dn_2 up_3 | imup_1 crdn_2 imup_3 @
        up_1: prev(imdn),
        up_3: min(prev()) < max(up_3),
        imup_1: prev(imdn),
        imup_3: min(prev()) < max(imup_3);
        
    
    