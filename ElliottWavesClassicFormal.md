# Classic Elliott Waves (Formal Description) #
Human-readable version can be found in [Elliott Waves Classic page](ElliottWavesClassic.md).

Latest version is in the _trunk_ folder of repository (see _data_ subfolder).

Current stable description (it is text of pat-file).

```
PATTERNS:
	#### COMMON PATTERNS ####
	
	#none

	#### MOTIVE WAVES ####
	
	# Upmoving impulse
	IMClassicUP = up_1 down_2 up_3 down_4 up_5 @
		(Min(down_4) >= Max(up_1)) and
		(Value(up_3) >= Value(up_1) or Value(up_3) >= Value(up_5)) and
		(Max(up_3) >= Max(up_1)) and
		(Min(down_2) >= Min(up_1)) and
		(Max(up_5) >= Max(up_3));
	
	IMFailedFifthUP = up_1 down_2 up_3 down_4 up_5 @
		(Min(down_4) >= Max(up_1)) and
		(Value(up_3) >= Value(up_1) or Value(up_3) >= Value(up_5)) and
		(Max(up_3) >= Max(up_1)) and
		(Min(down_2) >= Min(up_1)) and
		(Max(up_5) < Max(up_3));
	
	IMClassicUP = (IMUP_1 | LDUP_1) CRDN_2 IMUP_3 CRDN_4 (IMUP_5 | EDIAGUP_5) @
		(Min(CRDN_4) >= Max(*_1)) and 
		(Value(IMUP_3) >= Value(*_1) or Value(IMUP_3) >= Value(*_5)) and
		(Max(IMUP_3) >= Max(*_1)) and
		(Min(CRDN_2) >= Min(*_1)) and
		(Max(*_5) >= Max(IMUP_3));
		
	IMFailedFifthUP = (IMUP_1 | LDUP_1) CRDN_2 IMUP_3 CRDN_4 (IMUP_5 | EDIAGUP_5) @
		(Min(CRDN_4) >= Max(*_1)) and 
		(Value(IMUP_3) >= Value(*_1) or Value(IMUP_3) >= Value(*_5)) and
		(Max(IMUP_3) >= Max(*_1)) and
		(Min(CRDN_2) >= Min(*_1)) and
		(Max(*_5) < Max(IMUP_3));
		
	# Downmoving impulse
	IMClassicDN = down_1 up_2 down_3 up_4 (down_5 | EDIAGDN_5) @
		(Max(up_4) <= Min(down_1)) and 
		(Value(down_3) >= Value(down_1) or Value(down_3) >= Value(*_5)) and
		(Min(down_3) < Min(down_1)) and
		(Max(up_2) < Max(down_1)) and
		(Min(*_5) <= Min(down_3));
		
	IMClassicDN = (IMDN_1 | LDDN_1) CRUP_2 IMDN_3 CRUP_4 (IMDN_5 | EDIAGDN_5) @
		(Max(CRUP_4) <= Min(*_1)) and 
		(Value(IMDN_3) >= Value(*_1) or Value(IMDN_3) >= Value(*_5)) and
		(Max(IMDN_3) >= Max(*_1)) and
		(Max(CRUP_2) < Max(*_1)) and
		(Min(*_5) <= Min(IMDN_3));
	
	IMFailedFifthDN = down_1 up_2 down_3 up_4 (down_5 | EDIAGDN_5) @
		(Max(up_4) <= Min(down_1)) and 
		(Value(down_3) >= Value(down_1) or Value(down_3) >= Value(*_5)) and
		(Min(down_3) < Min(down_1)) and
		(Max(up_2) < Max(down_1)) and
		(Min(*_5) > Min(down_3));
		
	IMFailedFifthDN = (IMDN_1 | LDDN_1) CRUP_2 IMDN_3 CRUP_4 (IMDN_5 | EDIAGDN_5) @
		(Max(CRUP_4) <= Min(*_1)) and 
		(Value(IMDN_3) >= Value(*_1) or Value(IMDN_3) >= Value(*_5)) and
		(Max(IMDN_3) >= Max(*_1)) and
		(Max(CRUP_2) < Max(*_1)) and
		(Min(*_5) > Min(IMDN_3));


    ### Ending Diagonal (EDIAG) ###
	EDIAGClassicUP = 
		up_1 down_2 up_3 down_4 up_5 @
		Min(up_1) <= Min(up_3) and
		Min(up_3) <= Min(up_5) and
		Max(up_1) <= Max(up_3) and
		Max(up_3) <= Max(up_5) and
		AtOneLine(Min(up_1), StartTime(up_1),
				  Min(up_3), StartTime(up_3),
				  Min(up_5), StartTime(up_5),
				  5) and
		AtOneLine(Max(up_1), EndTime(up_1),
				  Max(up_3), EndTime(up_3),
				  Max(up_5), EndTime(up_5),
				  5);

	EDIAGClassicUP =
		up_1 down_2 up_3    
		down_4 up_5 down_6
		up_7 down_8 up_9
		down_10 up_11 down_12
		up_13 down_14 up_15
		@
		Min(up_1) <= Min(up_7) and
		Min(up_7) <= Min(up_13) and
		Max(up_3) <= Max(up_9) and
		Max(up_9) <= Max(up_15) and
		AtOneLine(Min(up_1), StartTime(up_1), 
				  Min(up_7), StartTime(up_7), 
				  Min(up_13), StartTime(up_13),
				  5) and
		AtOneLine(Max(up_3), EndTime(up_3),
				  Max(up_9), EndTime(up_9),
				  Max(up_15), EndTime(up_15),
				  5);
				  
	EDIAGClassicDN = 
		down_1 up_2 down_3 up_4 down_5 @
		Min(up_1) <= Min(up_3) and
		Min(up_3) <= Min(up_5) and
		Max(up_1) <= Max(up_3) and
		Max(up_3) <= Max(up_5) and
		AtOneLine(Min(up_1), StartTime(up_1),
				  Min(up_3), StartTime(up_3),
				  Min(up_5), StartTime(up_5),
				  5) and
		AtOneLine(Max(up_1), EndTime(up_1),
				  Max(up_3), EndTime(up_3),
				  Max(up_5), EndTime(up_5),
				  5);

	EDIAGClassicDN = 
		down_1 up_2 down_3    
		up_4 down_5 up_6
		down_7 up_8 down_9
		up_10 down_11 up_12
		down_13 up_14 down_15
		@
		AtOneLine(Max(down_1), StartTime(down_1), 
				  Max(down_7), StartTime(down_7), 
				  Max(down_13), StartTime(down_13),
				  5) and
		AtOneLine(Min(down_3), EndTime(down_3),
				  Min(down_9), EndTime(down_9),
				  Min(down_15), EndTime(down_15),
				  5);
	
	### Leading Diagonal Triangle (TLD) ###
	
	# Not implemented
		
	#### CORRECTIVE WAVES ####	
	
	## Common waves ##
	ZZXWaveUP = (up_1) | (up_1 down_2 up_3) | (IMUP_1 ZZSDN_2 IMUP_3) @
		IsPrev(ZZSDN);
	ZZXWaveDN = (down_1) | (down_1 up_2 down_3) | (IMDN_1 ZZSUP_2 IMDN_3) @
		IsPrev(ZZSUP);
	
	## Zigzag (ZZ) ##
	
	# Single Zigzag (ZZS) #
	ZZSDN = down_1 up_2 down_3 @ 
			(IsPrev(IMUP) or IsPrev(ZZXWaveUP) or IsPrev(ZZSDN) or IsPrev(ZZSUP)) and
			and Min(self()) >= Max(prev()) and Max(up_2) < Max(down_1);
	ZZSDN = down_1 up_2 down_3 up_4 down_5 up_6 down_7 up_8 down_9 up_10 down_11 up_12 down_13 @ 
			(IsPrev(IMUP) or IsPrev(ZZXWaveUP) or IsPrev(ZZSDN) or IsPrev(ZZSUP)) and
			and Min(self()) >= Max(prev()) and Max(up_8) < Max(down_1);		
	ZZSDN = IMDN_1 ZZSUP_2 IMDN_3 @
			(IsPrev(IMUP) or IsPrev(ZZXWaveUP) or IsPrev(ZZSDN) or IsPrev(ZZSUP)) and
			and Min(self()) >= Max(prev()) and Max(ZZSUP_2) < Max(IMDN_1);
	
	ZZSUP = up_1 down_2 up_3 @ 
			(IsPrev(IMDN) or IsPrev(ZZXWaveDN) or IsPrev(ZZSDN) or IsPrev(ZZSUP)) and
			and Max(self()) <= Max(prev()) and Min(down_2) > Min(up_1);
	ZZSUP = up_1 down_2 up_3 down_4 up_5 down_6 up_7 down_8 up_9 down_10 up_11 down_12 up_13 @ 
			(IsPrev(IMDN) or IsPrev(ZZXWaveDN) or IsPrev(ZZSDN) or IsPrev(ZZSUP)) and
			and Max(self()) <= Max(prev()) and Min(down_8) > Min(up_1);
	ZZSUP = IMUP_1 ZZSDN_2 IMUP_3 @
			(IsPrev(IMDN) or IsPrev(ZZXWaveDN) or IsPrev(ZZSDN) or IsPrev(ZZSUP)) and
			and Max(self()) <= Max(prev()) and Min(ZZSDN_2) > Min(IMUP_1);

			
	# Double zigzag (ZZD) #
	ZZDDN = ZZSDN_1 ZZXWaveUP_2 ZZSDN_3 @ true;
	ZZDUP = ZZSUP_1 ZZXWaveDN_2 ZZSUP_3 @ true;
	
	# Triple zigzag (ZZT) #
	ZZTDN = ZZSDN_1 ZZXWaveUP_2 ZZSDN_3 ZZXWaveUP_4 ZZSDN_5 @ true;
	ZZTUP = ZZSUP_1 ZZXWaveDN_2 ZZSUP_3 ZZXWaveDN_4 ZZSUP_5 @ true;
	

	## FLATS ##
	
	# Generic flat (FLATGeneric, contains Regular, Running and Expanded #
	FLATRegularDN = (down_1 | ZZSDN_1) (up_2 | ZZSUP_2) (down_3 | IMDN_3) @
		IsPrev(IMUP) and
		Near(Max(*_1), Max(*_2), Value(*_2)*0.1) and 
		Near(Min(*_3), Min(*_2), Value(*_2)*0.1);
		
	FLATRegularUP = (up_1 | ZZSUP_1) (down_2 | ZZSDN_2) (up_3 | ZZSUP_3) @
		IsPrev(IMUP) and 
		Near(Min(*_1) - Min(*_2), Value(*_1)*0.1) and 
		Near(Max(*_3) - Min(*_2), Value(*_1)*0.1);
	
	## TRIANGLES ##
	
	TRIUP = up_1 down_2 up_3 down_4 @
		IsPrev(IMDN) and
		Min(down_2) >= Min(up_1) and 
		Max(up_3) <= Max(up_1) and
		Min(down_4) >= Min(down_2) and
		AtOneLine(
			StartTime(up_1), Min(up_1),
			StartTime(up_3), Min(up_3),
			EndTime(down_4), Min(down_4),
			2);

	TRIUP = up_1 down_2 up_3 down_4 up_5 down_6 up_7 down_8 up_9 down_10 up_11 down_12 @
		IsPrev(IMDN) and
		Min(down_6) >= Min(up_1) and 
		Max(up_9) <= Max(up_3) and
		Min(down_12) >= Min(down_6) and
		AtOneLine(
			StartTime(up_1), Min(up_1),
			StartTime(up_7), Min(up_7),
			EndTime(down_12), Min(down_12),
			10);
			
	TRIUP = ZZUP_1 ZZDN_2 ZZUP_3 (ZZDN_4 | FLATRegularDN_4) @
		IsPrev(IMDN) and
		Min(ZZDN_2) >= Min(ZZUP_1) and 
		Max(ZZUP_3) <= Max(ZZUP_1) and
		Min(*_4) >= Min(ZZDN_2) and
		AtOneLine(
			StartTime(ZZUP_1), Min(ZZUP_1),
			StartTime(ZZUP_3), Min(ZZUP_3),
			EndTime(*_4), Min(*_4),
			2);

	TRIDN = down_1 up_2 down_3 up_4 @
		IsPrev(IMUP) and
		Max(up_2) <= Max(down_1) and 
		Min(down_3) >= Min(down_1) and
		Max(up_4) <= Max(down_1) and
		AtOneLine(
			StartTime(down_1), Max(down_1),
			StartTime(down_3), Max(down_3),
			EndTime(up_4), Max(up_4),
			2);

	TRIDN = down_1 up_2 down_3 up_4 down_5 up_6 down_7 up_8 down_9 up_10 down_11 up_12 @
		IsPrev(IMUP) and
		Max(up_6) <= Max(down_1) and 
		Min(down_9) >= Min(down_3) and
		Max(up_12) <= Max(down_1) and
		AtOneLine(
			StartTime(down_1), Max(down_1),
			StartTime(down_7), Max(down_7),
			EndTime(up_12), Max(up_12),
			2);
			
	TRIDN = ZZSDN_1 ZZSUP_2 ZZSDN_3 (ZZSUP_4 | FLATRunningUP_4) @
		IsPrev(IMUP) and
		Max(up_2) <= Max(down_1) and 
		Min(down_3) >= Min(down_1) and
		Max(up_4) <= Max(down_1) and
		AtOneLine(
			StartTime(down_1), Max(down_1),
			StartTime(down_3), Max(down_3),
			EndTime(up_4), Max(up_4),
			2);
	
SYNONYMS:
	IMUP = IMClassicUP;
	IMUP = IMFailedFifthUP;
	IMDN = IMClassicDN;
	IMDN = IMFailedFifthDN;
	
	EDIAGUP = EDIAGClassicUP;
	EDIAGUP = EDIAGClassicUP;
	EDIAGDN = EDIAGClassicDN;
	EDIAGDN = EDIAGClassicDN;
```