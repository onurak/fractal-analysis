PATTERNS:
        
        ##################################
        # MOTIVE WAVES
        ##################################

        IMClassicUP = IMUP_1 CRDN_2 IMUP_3 CRDN_4 IMUP_5 @
              CRDN_2: min(CRDN_2) >= min(IMUP_1),
              IMUP_3: max(IMUP_3) >= max(IMUP_1),
              CRDN_4: min(CRDN_4) >= max(IMUP_1),
              IMUP_5: (value(IMUP_3) >= value(IMUP_1) or value(IMUP_3) >= value(IMUP_5))
                    and
                    (max(IMUP_5) >= max(IMUP_3));
        
#        IMFailedFifthUP = IMUP_1 CRDN_2 IMUP_3 CRDN_4 IMUP_5 @
#                CRDN_4: min(CRDN_4) >= max(IMUP_1),
#                IMUP_3: max(IMUP_3) >= max(IMUP_1),
#                IMUP_5: (value(IMUP_3) >= value(IMUP_1) or value(IMUP_3) >= value(IMUP_5)) and
#                      max(IMUP_5) < max(IMUP_3),
#                CRDN_2: min(CRDN_2) >= min(IMUP_1);


        IMClassicDN = IMDN_1 CRUP_2 IMDN_3 CRUP_4 IMDN_5 @
              CRUP_2: max(CRUP_2) <= max(IMDN_1),
              IMDN_3: min(IMDN_3) <= min(IMDN_1),
              CRUP_4: max(CRUP_4) <= min(IMDN_1),
              IMDN_5: (value(IMDN_3) >= value(IMDN_1) or value(IMDN_3) >= value(IMDN_5))
                    and
                    (min(IMDN_5) <= min(IMDN_3));
        
#        IMFailedFifthDN = IMDN_1 CRUP_2 IMDN_3 CRUP_4 IMDN_5 @
#              CRUP_2: max(CRUP_2) <= max(IMDN_1),
#              IMDN_3: min(IMDN_3) <= min(IMDN_1),
#              CRUP_4: max(CRUP_4) <= min(IMDN_1),
#              IMDN_5: (value(IMDN_3) >= value(IMDN_1) or value(IMDN_3) >= value(IMDN_5))
#                    and
#                    (min(IMDN_5) >= min(IMDN_3));


        LeadingDiagUP = IMUP_1 CRDN_2 IMUP_3 CRDN_4 IMUP_5 @
              IMUP_5: IsLine( end(IMUP_1), max(IMUP_1),
                              end(IMUP_1), max(IMUP_1), 


        ##################################
        # CORRECTIVE WAVES
        ##################################

        ZZDN = ZZDNStart_1   ZZDNMiddle_2   ZZDNEnd_3 @
              ZZDNStart_1:   prev(ZZDNAllowedPrev),
              ZZDNMiddle_2:  (max(ZZDNMiddle_2) < max(ZZDNStart_1)) and (value(ZZDNMiddle_2) < value(ZZDNStart_1)),
              ZZDNEnd_3:     min(ZZDNEnd_3) < min(ZZDNMiddle_2);

        ZZUP = ZZUPStart_1   ZZUPMiddle_2   ZZUPEnd_3 @
              ZZUPStart_1:   prev(ZZUPAllowedPrev),
              ZZUPMiddle_2:  (min(ZZUPMiddle_2) > min(ZZUPStart_1)) and (value(ZZUPMiddle_2) < value(ZZUPStart_1)),
              ZZUPEnd_3:     max(ZZUPEnd_3) > max(ZZUPMiddle_2);



        RegularFlatDN = RegularFlatDNStart_1  RegularFlatDNMiddle_2  RegularFlatDNEnd_3 @
              RegularFlatDNStart_1:    prev(RegularFlatDNAllowedPrev),
              RegularFlatDNMiddle_2:   abs(max(RegularFlatDNStart_1) - max(RegularFlatDNMiddle_2)) < 0.1*value(RegularFlatDNMiddle_2),
              RegularFlatDNEnd_3:      abs(min(RegularFlatDNEnd_3) - min(RegularFlatDNMiddle_2)) < 0.1*value(RegularFlatDNMiddle_2);

        RegularFlatUP = RegularFlatUPStart_1  RegularFlatUPMiddle_2  RegularFlatUPEnd_3 @
              RegularFlatUPStart_1:    prev(RegularFlatUPAllowedPrev),
              RegularFlatUPMiddle_2:   abs(max(RegularFlatUPStart_1) - max(RegularFlatUPMiddle_2)) < 0,
              RegularFlatUPEnd_3:      abs(min(RegularFlatUPEnd_3) - min(RegularFlatUPMiddle_2)) < 0;


SYNONYMS:
  IMUP = up, IMClassicUP, IMFailedFifthUP, LeadingDiagUP;
  IMDN = dn, IMClassicDN, IMFailedFifthDN, LeadingDiagDN;
  CRUP = up, ZZUP, DoubleZZUP, RegularFlatUP, RuningFlatUP;
  CRDN = dn, ZZDN, DoubleZZDN, RegularFlatDN, RuningFlatDN;

  ZZDNAllowedPrev = IMClassicUP, IMFailedFifthUP, LeadingDiagUP;
  ZZDNStart = dn, IMClassicDN;
  ZZDNMiddle = up, ZZUP;
  ZZDNEnd = dn, IMClassicDN;

  ZZUPAllowedPrev = IMClassicDN, IMFailedFifthDN, LeadingDiagDN;
  ZZUPStart = up, IMClassicUP;
  ZZUPMiddle = dn, ZZDN;
  ZZUPEnd = up, IMClassicUP;

  RegularFlatDNAllowedPrev = IMClassicUP, IMFailedFifthUP, LeadingDiagUP;
  RegularFlatDNStart = dn, ZZDN;
  RegularFlatDNMiddle = up, ZZUP;
  RegularFlatDNEnd = dn, IMClassicDN;


  RegularFlatUPAllowedPrev = IMClassicDN, IMFailedFifthDN, LeadingDiagDN;
  RegularFlatUPStart = up, ZZUP;
  RegularFlatUPMiddle = dn, ZZDN;
  RegularFlatUPEnd = up, IMClassicUP;
