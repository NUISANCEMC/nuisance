#!/bin/sh

opt=$1
fullcard=$2

if [[ card == "-copy" || card == "-clean"  || card == "-make" ]]
then
    echo "Switching"
    opt=$2
    fullcard=$1
fi

card=$(basename $fullcard)
fulldir=$(dirname $fullcard)
if [[ $card != ${fullcard} && $fulldir != $PWD ]]
then
    cp $fullcard $card
fi

allfiles=""
if [[ $opt == "-make" ]]
then
    if [[ -e "$card" ]]
    then
	allfiles=""
#        echo "Copying files across from $card"
    else
        echo ""
        return
    fi

    # Copy File                                                                                                                                                                                                                             
    cp $card preload_${card}

    # Strip out config replacers                                                                                                                                                                                                            
    for obj in $(\grep config $card);
    do
        for events in "NEUT_DIR" "GENIE_DIR" "NUWRO_DIR" "NUSANCE_DIR" "EVSPLN_DIR" "GIBUU_DIR"
        do

            if [[ $obj == ${events}"="* ]]
            then
                obj=${obj//\/\>/}
                obj=${obj//${events}=/}
                obj=${obj//\"/}

#                echo "Sedding : " sed -i -e "s#$events#$obj#g" preload_${card}
                sed -i -e "s#@$events#$obj#g" preload_${card}
            fi
        done
    done

    # Make preload                                                                                                                                                                                                                          
    for obj in $(\grep input preload_${card});
    do

        if [[ $obj != "input="* ]]
        then
            continue
        fi

        file=${obj//\"/}
        file=${file//input=/}
        file=${file//\/\>/}
        file=${file//\.\//}

        file=${file//\(/ }
        file=${file//\)/ }
        file=${file//\,/ }
        file=${file//\;/ }

        file=${file//GENIE:/ }
        file=${file//NEUT:/ }
        file=${file//NUWRO:/ }
        file=${file//ROOT:/ }
        file=${file//FEVENT:/ }

	for newfile in $file;
        do
            base=$(basename $newfile)
            if [[ "$base" != "$newfile" ]]
            then
                sed -i -e "s#$newfile#$base#g" preload_${card}
            fi
	    allfiles=$newfile,$allfiles
        done
    done
    echo $allfiles
fi


# Copy Files
if [[ $opt == "-copy" ]]
then
    
    if [[ -e "$card" ]]
    then
        echo "Copying files across from $card"
    else
	echo "$card not found!"
	return
    fi
    
    # Copy File
    cp $card preload_${card}

    # Strip out config replacers
    for obj in $(\grep config $card);
    do
	for events in "NEUT_DIR" "GENIE_DIR" "NUWRO_DIR" "NUSANCE_DIR" "EVSPLN_DIR" "GIBUU_DIR" 
	do
	    
	    if [[ $obj == ${events}"="* ]]
	    then
		obj=${obj//\/\>/}
		obj=${obj//${events}=/}
		obj=${obj//\"/}
		
		echo "Sedding : " sed -i -e "s#$events#$obj#g" preload_${card} 
		sed -i -e "s#@$events#$obj#g" preload_${card}
	    fi
	done
    done

    # Make preload
    for obj in $(\grep input preload_${card});
    do 
	
	if [[ $obj != "input="* ]] 
	then
	    continue
	fi

	file=${obj//\"/}
	file=${file//input=/}
	file=${file//\/\>/}
	file=${file//\.\//}

	file=${file//\(/ }
	file=${file//\)/ }
	file=${file//\,/ }
	file=${file//\;/ }
	
	file=${file//GENIE:/ }
	file=${file//NEUT:/ }
	file=${file//NUWRO:/ }
	file=${file//ROOT:/ }
	file=${file//FEVENT:/ }

	for newfile in $file;
	do
	    base=$(basename $newfile)
	    if [[ "$base" != "$newfile" ]]
            then
                #echo cp $newfile $base
		cp -rvf $newfile $base
		sed -i -e "s#$newfile#$base#g" preload_${card}
            else
		echo "Not copying $base $newfile"
            fi
	done
    done
fi

if [[ $opt == "-clean" ]]
then
    
    if [[ -e "preload_${card}" ]] 
    then
	echo "Cleaning from preload_${card}"
    else
	echo "No preload card found!" 
	return 0
    fi

    # Strip out config replacers
    for obj in $(\grep config $card);
    do
        for events in "NEUT_DIR" "GENIE_DIR" "NUWRO_DIR" "NUSANCE_DIR" "EVSPLN_DIR" "GIBUU_DIR"
        do

            if [[ $obj == ${events}"="* ]]
            then
                obj=${obj//\/\>/}
                obj=${obj//${events}=/}
                obj=${obj//\"/}

                echo "Sedding : " sed -i -e "s#$events#$obj#g" preload_${card}
                sed -i -e "s#@$events#$obj#g" preload_${card}
            fi
        done
    done

    # Make preload
    for obj in $(\grep input ${card});
    do
        if [[ $obj != "input="* ]]
        then
            continue
        fi

        file=${obj//\"/}
        file=${file//input=/}
        file=${file//\/\>/}
	file=${file//\.\//}

        file=${file//\(/ }
        file=${file//\)/ }
        file=${file//\,/ }
        file=${file//\;/ }

        file=${file//GENIE:/ }
        file=${file//NEUT:/ }
        file=${file//NUWRO:/ }
        file=${file//ROOT:/ }
	file=${file//FEVENT:/ }

        for newfile in $file;
        do
            base=$(basename $newfile)
	    if [[ "$base" != "$newfile" ]]
	    then
#		echo rm $base
		rm -v $base
	    else
		echo "Not removing current folder file : $base $newfile"
	    fi
        done
    done

    # Remove file
    rm preload_${card}

    if [[ $card != $fullcard ]]
    then
	rm $card
    fi
fi

