#!/bin/sh

opt=$1
card=$2

if [[ card == "-copy" || card == "-clean" ]]
then
    echo "Switching"
    opt=$2
    card=$1
fi


# Copy Files
if [[ $opt == "-copy" ]]
then
    
    echo "Copying files across"
    
    # Copy File
    cp $card preload_${card}

    # Strip out config replacers
    for obj in $(grep config $card);
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
    for obj in $(grep input preload_${card});
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
    
	for newfile in $file;
	do
	    base=$(basename $newfile)
	    if [[ "$base" != "$newfile" ]]
            then
                echo cp $newfile $base
		#cp -rvf $newfile $base
		sed -i -e "s#$newfile#$base#g" preload_${card}
            else
		echo "Not copying $base $newfile"
            fi
	done
    done
fi

if [[ $opt == "-clean" ]]
then
    
    # Strip out config replacers
    for obj in $(grep config $card);
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
    for obj in $(grep input ${card});
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

        for newfile in $file;
        do
            base=$(basename $newfile)
	    if [[ "$base" != "$newfile" ]]
	    then
		echo rm $base
		#rm $base
	    else
		echo "Not removing current folder file : $base $newfile"
	    fi
        done
    done

fi

