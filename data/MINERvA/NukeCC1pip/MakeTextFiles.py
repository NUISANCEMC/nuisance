lines = open('Table_Var_Xsec.csv').readlines()

Variables = [
"$T_{\pi}$",
"$p_{\mu,T}$",
]

Targets = [
"scintillator",
"iron",
"lead",
]

for Variable in Variables:

  VariableName = ""
  if Variable=="$T_{\pi}$":
    VariableName = "Tpi"
  if Variable=="$p_{\mu,T}$":
    VariableName = "ptmu"

  for Target in Targets:

    TargetName = ""
    if Target=="scintillator":
      TargetName = "CH"
    if Target=="iron":
      TargetName = "Fe"
    if Target=="lead":
      TargetName = "Pb"

    for i_line,line in enumerate(lines):

      ## Find the first line
      ## e.g.) Measured cross section as function of $T_{\pi}$ on iron, in units of $10^{-42}$ $\text{cm}^2$/GeV/nucleon, and the absolute and fractional cross section uncertainties
      if "Measured cross section" in line and "as function of %s"%(Variable) in line and "on %s"%(Target) in line:

        output_common_name = "NukeCC1pip_%s_%s"%(TargetName, VariableName)
        output_data = open("%s.txt"%(output_common_name),"w")
        output_cov_stat = open("%s_cov_stat.txt"%(output_common_name),"w")
        output_cov_syst = open("%s_cov_syst.txt"%(output_common_name),"w")
        output_cov_total = open("%s_cov_total.txt"%(output_common_name),"w")

        ## Measured
        print("@@ %s %s found"%(Variable, Target))
        line_start_Measured = i_line+3
        line_end_Measured = line_start_Measured+1
        for j_line in range(line_start_Measured, len(lines)):
          if lines[j_line]=="\n":
            line_end_Measured = j_line
            break

        ## Find  bins first
        bins = []
        for j_line in range(line_start_Measured,line_end_Measured):
          words = lines[j_line].strip('\n').split(';')
          out = ''
          bin_range = words[0]
          bin_l = bin_range.split('-')[0]
          bin_r = bin_range.split('-')[1]
          if len(bins)==0:
            bins.append(bin_l)
          bins.append(bin_r)

        NColumn = 0
        RowCounter = 0

        ForValidation_Measured = []
        for j_line in range(line_start_Measured,line_end_Measured):
          words = lines[j_line].strip('\n').split(';')
          out = bins[RowCounter]+' '
          NColumn = len(words)-1
          ThisValues = []
          for i_w in range(1, len(words)):
            this_word = words[i_w]
            this_word = this_word.replace(' ','')
            out += this_word
            ThisValues.append(float(this_word))
            if i_w != len(words)-1:
              out += ' '
          output_data.write(out+'\n')
          ForValidation_Measured.append(ThisValues)
          RowCounter += 1
        output_data.write(bins[-1]+" 0."*NColumn+'\n')
        output_data.close()

        ## Stat cov
        line_start_StatCov = line_end_Measured+4
        line_end_StatCov = line_start_StatCov+1
        for j_line in range(line_start_StatCov, len(lines)):
          if lines[j_line]=="\n":
            line_end_StatCov = j_line
            break

        ForValidation_StatCov = []
        for j_line in range(line_start_StatCov,line_end_StatCov):
          words = lines[j_line].strip('\n').split(';')
          out = ''
          ThisValues = []
          for i_w in range(1, len(words)):
            this_word = words[i_w]
            this_word = this_word.replace(' ','')
            out += this_word
            ThisValues.append(float(this_word))
            if i_w != len(words)-1:
              out += ' '
          output_cov_stat.write(out+'\n')
          ForValidation_StatCov.append(ThisValues)
        output_cov_stat.close()

        ## Syst cov
        line_start_SystCov = line_end_StatCov+4
        line_end_SystCov = line_start_SystCov+1
        for j_line in range(line_start_SystCov, len(lines)):
          if lines[j_line]=="\n":
            line_end_SystCov = j_line
            break

        ForValidation_SystCov = []
        for j_line in range(line_start_SystCov,line_end_SystCov):
          words = lines[j_line].strip('\n').split(';')
          out = ''
          ThisValues = []
          for i_w in range(1, len(words)):
            this_word = words[i_w]
            this_word = this_word.replace(' ','')
            out += this_word
            ThisValues.append(float(this_word))
            if i_w != len(words)-1:
              out += ' '
          output_cov_syst.write(out+'\n')
          ForValidation_SystCov.append(ThisValues)
        output_cov_syst.close()

        ## Sum
        # ForValidation_Measured
        # ForValidation_StatCov
        # ForValidation_SystCov

        for i_row in range(0, len(ForValidation_StatCov)):

          line_SumCov = ""

          Row_Measured = ForValidation_Measured[i_row]
          Row_StatCov = ForValidation_StatCov[i_row]
          Row_SystCov = ForValidation_SystCov[i_row]

          TotalUncFromData = Row_Measured[1]

          for i_col in range(0, len(Row_StatCov)):
            val_SyatCov = Row_StatCov[i_col]
            val_SystCov = Row_SystCov[i_col]
            val_TotalCov = val_SyatCov+val_SystCov
            if i_col==i_row:
              CovFromData = TotalUncFromData*TotalUncFromData
              RelDiff = abs(CovFromData-val_TotalCov)/CovFromData
              if RelDiff>1E-3:
                raise

            line_SumCov += "%1.3f"%(val_TotalCov)
            if i_col != len(Row_StatCov)-1:
              line_SumCov += ' '
          output_cov_total.write(line_SumCov+'\n')
        output_cov_total.close()

            

        break
