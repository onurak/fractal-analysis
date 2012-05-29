PATTERNS:
        
        ##################################
        # MOTIVE WAVES
        ##################################

        IMClassicUP = IMUP1_1 CRDN_2 IMUP3_3 CRDN_4 IMUP5_5 @
              CRDN_2: min(CRDN_2) >= min(IMUP1_1),
              IMUP3_3: max(IMUP3_3) >= max(IMUP1_1),
              CRDN_4: min(CRDN_4) >= max(IMUP1_1),
              IMUP5_5: (value(IMUP3_3) >= value(IMUP1_1) or value(IMUP3_3) >= value(IMUP5_5))
                       ;

        IMClassicDN = IMDN1_1 CRUP_2 IMDN3_3 CRUP_4 IMDN5_5 @
              CRUP_2: max(CRUP_2) <= max(IMDN1_1),
              IMDN3_3: min(IMDN3_3) <= min(IMDN1_1),
              CRUP_4: max(CRUP_4) <= min(IMDN1_1),
              IMDN5_5: (value(IMDN3_3) >= value(IMDN1_1) or value(IMDN3_3) >= value(IMDN5_5))
                    ;


#        LeadingDiagUP = LDUP_1 CRDN_2 LDUP_2 CRDN_4 LDUP_3 @
#              CRDN_2: min(CRDN_2) >= min(LDUP_1),
#              LDUP_3: max(LDUP_3) >= max(LDUP_1),
#              CRDN_4: min(CRDN_4) >= min(LDUP_3) and
#                      (TriangleType( end(LDUP_1), max(LDUP_1),
#                                     end(LDUP_3), max(LDUP_3),
#                                     end(CRDN_2), min(CRDN_2),
#                                     end(CRDN_4), min(CRDN_4) ) == 1),
#              LDUP_5: max(LDUP_5) > max(LDUP_3);



#        LeadingDiagDN = LDDN_1 CRUP_2 LDDN_2 CRUP_4 LDDN_3 @
#              CRUP_2: max(CRUP_2) <= max(LDDN_1),
#              LDDN_3: min(LDDN_3) <= min(LDDN_1),
#              CRUP_4: max(CRUP_4) <= max(LDDN_3) and
#                      (TriangleType( end(LDDN_1), max(LDDN_1),
#                                     end(LDDN_3), max(LDDN_3),
#                                     end(CRUP_2), min(CRUP_2),
#                                     end(CRUP_4), min(CRUP_4) ) == 1),
#              LDDN_5: min(LDDN_5) < min(LDDN_3);


#              cost: +5,
#              direction forecast: up,
#              value forecast: from 0 to value(IMUP5_5),
#              time forecast: from 0,
#              visual:   line(end(IMUP1_1), max(IMUP1_1), end(IMUP3_3), max(IMUP3_3)),
#                        line(end(CRDN_2), max(CRDN_2), end(CRDN_4), max(CRDN_4));


        ##################################
        # CORRECTIVE WAVES
        ##################################

        ZZDN = ZZDN1_1   ZZDN2_2   ZZDN3_3 @
              *_*:      length(self()) <= length(prev()),
              ZZDN1_1:  prev(ZZDNAllowedPrev),
              ZZDN2_2:  (max(ZZDN2_2) < max(ZZDN1_1)) and (value(ZZDN2_2) < value(ZZDN1_1));

        ZZUP = ZZUP1_1   ZZUP2_2   ZZUP3_3 @
              *_*:      length(self()) <= length(prev()),
              ZZUP1_1:  prev(ZZUPAllowedPrev),
              ZZUP2_2:  (min(ZZUP2_2) > min(ZZUP1_1)) and (value(ZZUP2_2) < value(ZZUP1_1));

        ZZXUP = dn | ZZUP @
              *_*: isprev(ZZDN);

        DoubleZZDN = ZZDN1_1   ZZXUP_2   ZZDN3_3 @
              ZZDN1_1:  prev(ZZDNAllowedPrev),
              ZZXUP_2:  (max(ZZXUP_2) < max(ZZDN1_1)) and (value(ZZXUP_2) < value(ZZDN1_1));


        ContractingTriangleDN = CTDN_1 CTDN_2 CTDN_3 CTDN_4 CTDN_5 @
              CTDN_1: isprev(IMUP),
              CTDN_4: (TriangleType( end(CTDN_1), max(CTDN_1),
                                     end(CTDN_3), max(CTDN_3),
                                     end(CTDN_2), min(CTDN_2),
                                     end(CTDN_4), min(CTDN_4) ) == 1);

        ContractingTriangleUP = CTUP_1 CTUP_2 CTUP_3 CTUP_4 CTUP_5 @
              CTUP_1: isprev(IMDN),
              CTUP_4: (TriangleType( end(CTUP_1), max(CTUP_1),
                                     end(CTUP_3), max(CTUP_3),
                                     end(CTUP_2), min(CTUP_2),
                                     end(CTUP_4), min(CTUP_4) ) == 1);


        RegularFlatDN = RegularFlatDN1_1  RegularFlatDN2_2  RegularFlatDN3_3 @
              *_*:                length(self()) <= length(prev()),
              RegularFlatDN1_1:   prev(RegularFlatDNAllowedPrev),
              RegularFlatDN2_2:   abs(max(RegularFlatDN1_1) - max(RegularFlatDN2_2)) < 0.1*value(RegularFlatDN2_2),
              RegularFlatDN3_3:   abs(min(RegularFlatDN3_3) - min(RegularFlatDN2_2)) < 0.1*value(RegularFlatDN2_2);

        RegularFlatUP = RegularFlatUP1_1  RegularFlatUP2_2  RegularFlatUP3_3 @
              *_*:                length(self()) <= length(prev()),
              RegularFlatUP1_1:   prev(RegularFlatUPAllowedPrev),
              RegularFlatUP2_2:   abs(max(RegularFlatUP1_1) - max(RegularFlatUP2_2)) < 0.1*value(RegularFlatDN2_2),
              RegularFlatUP3_3:   abs(min(RegularFlatUP3_3) - min(RegularFlatUP2_2)) < 0.1*value(RegularFlatDN2_2);


SYNONYMS:
  IMUP1 = up, IMClassicUP, IMFailedFifthUP, LeadingDiagUP;
  IMUP3 = up, IMClassicUP, IMFailedFifthUP;
  IMUP5 = up, IMClassicUP, IMFailedFifthUP, EndingDiagUP;

  IMDN1 = dn, IMClassicDN, IMFailedFifthDN, LeadingDiagDN;
  IMDN3 = dn, IMClassicDN, IMFailedFifthDN;
  IMDN5 = dn, IMClassicDN, IMFailedFifthDN, EndingDiagDN;

  CRUP = up, ZZUP, DoubleZZUP, RegularFlatUP, RuningFlatUP;
  CRDN = dn, ZZDN, DoubleZZDN, RegularFlatDN, RuningFlatDN;

  LDUP = IMClassicUP, IMFailedFifthUP, LeadingDiagUP, EndingDiagUP;
  LDDN = IMClassicDN, IMFailedFifthDN, LeadingDiagDN, EndingDiagDN;
  

  ZZDNAllowedPrev = IMClassicUP, IMFailedFifthUP, LeadingDiagUP;
  ZZDN1 = dn, IMClassicDN;
  ZZDN2 = up, ZZUP;
  ZZDN3 = dn, IMClassicDN;

  ZZUPAllowedPrev = IMClassicDN, IMFailedFifthDN, LeadingDiagDN;
  ZZUP1 = up, IMClassicUP;
  ZZUP2 = dn, ZZDN;
  ZZUP3 = up, IMClassicUP;

  CTUP = up, ZZUP, DoubleZZUP;
  CTDN = dn, ZZDN, DoubleZZDN;

  RegularFlatDNAllowedPrev = IMClassicUP, IMFailedFifthUP, LeadingDiagUP;
  RegularFlatDN1 = dn, ZZDN;
  RegularFlatDN2 = up, ZZUP;
  RegularFlatDN3 = dn, IMClassicDN;


  RegularFlatUPAllowedPrev = IMClassicDN, IMFailedFifthDN, LeadingDiagDN;
  RegularFlatUP1 = up, ZZUP;
  RegularFlatUP2 = dn, ZZDN;
  RegularFlatUP3 = up, IMClassicUP;
