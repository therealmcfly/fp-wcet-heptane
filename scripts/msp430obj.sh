#!/bin/sh

filename=$1
b_TRACE_="false"
#b_TRACE_="true"

function _TRACE_ {
 if [ "$b_TRACE_" == "true" ]
   then
     echo "$1"
     echo "$2"
     echo "$3"
 fi;
}

declare global LIGNE

function getNumLine() {
    local curElem=$1
    local vaddr="$(echo ${curElem}:)"
    local linevaddr="$(grep -n ${vaddr} $filename)"
    local u="$(expr index "$linevaddr" ':')"
    local s="$(echo ${linevaddr: 0 : $u-1 })"
    LIGNE=$(($s - 1))
}

declare global NAME
function getNameEntryPoint()
{
    local ep=$1
    local b="$(expr index "$ep" '<')"
    local e="$(expr index "$ep" '>')"
    NAME="$(echo ${ep: $b : $e-$b-1})"
}

function revoveEntryPoint()
{
    local entryPoint=$1
    local NextEntryPoint=$2
    # echo " revoveEntryPoint  current=$entryPoint , next=$NextEntryPoint"
    getNumLine $entryPoint 
    local l1=$LIGNE
    local BeginEntryPoint="$(sed -n "${l1} p" $filename)"
    # echo " Begin at line $l1,  $BeginEntryPoint" 

    getNumLine $NextEntryPoint
    local l2=$(($LIGNE - 1))
    if [ "${l2}"  == "0" ]; then
      local  EndEntryPoint="$(sed -n "\$ p" $filename)"
      sed -i "${l1},\$ d" $filename
      # echo " End at EOF, $EndEntryPoint"
    else
       local  EndEntryPoint="$(sed -n "${l2} p" $filename)"
       sed -i "${l1},${l2} d" $filename	
      # echo " End at line   $l2,  $EndEntryPoint"
    fi

    getNameEntryPoint "$BeginEntryPoint" 
    echo " >>> Deleting the entry point = $NAME"
}

declare -a TABCALLS

function isInTABCALLS {
    for var in "${!TABCALLS[@]}" ; do      
	if [ "${TABCALLS[$var]}"  == "$1" ]; then
	    return 1
	fi
    done   
    return 0
}

function insertCall 
{
    local trouve=0
    local b="$(expr index  "$1" '<')"
    local vaddr="$(echo ${1: 0 : ${b}-1})"
     # removing #Ox"
    vaddr="$(echo "${vaddr#\#}")"
    vaddr="$(echo "${vaddr#0x}")"
    isInTABCALLS "$vaddr"
    if [ $? == 0 ] ; then
	TABCALLS[${#TABCALLS[*]}]=$vaddr
    fi
}

# Print the table of the calls
function printCalls {

    for var in "${!TABCALLS[@]}" ; do      
	echo " TABCALLS[ $var ] = ${TABCALLS[$var]}"        
    done  
}

declare -a TABFUNCTIONS

# Print the TABLE of FUNCTIONS
function printFunctions {
    for var in "${!TABFUNCTIONS[@]}" ; do      
	echo "TABFUNCTIONS[ $var ]= ${TABFUNCTIONS[$var]}"        
    done 
}


# The "br instructions"
declare -a TABGOTO

function isInTABGOTOS {
    for var in "${!TABGOTO[@]}" ; do      
	if [ "${TABGOTO[$var]}"  == "$1" ]; then
	    return 1
	fi
    done   
    return 0
}

function insertGoto {
    local found="$(echo $1 | grep -e br)"
    if [ -n "$found" ]
     then 
	b="$(expr index "$1" 'br')"
	vaddr="$(echo ${1:${b}+2})"
    else
	vaddr=$1
     fi
    local vaddr="$(echo "${vaddr#\#}")"
    vaddr="$(echo "${vaddr#0x}")"
    isInTABGOTOS "$vaddr"
    if [ $? == 0 ] ; then
	TABGOTO[${#TABGOTO[*]}]=$vaddr
    fi
}

# Print the table of the GOTO
function printGotos {
    for var in "${!TABGOTO[@]}" ; do      
	echo "TABGOTO[ $var ]= ${TABGOTO[$var]}"        
    done 
}

echo "Begin script on objdump"

# 2 problemes : no CFG for jumps, register and #addr for call

###	JUMP	###  
  jvar="$(grep ' <.L' $1)"
  #echo $jvar
  
  j=1
  temp=1
while [ $temp -le 1 ]
do
  SUBS=$(echo $jvar| cut -d':' -f $j)
  if [ -z "$SUBS" ]
  then
      temp=2
  else
      uselesseq=${SUBS:9:16}
      #echo $uselesseq
      sed -i "/$uselesseq/d" $1
      j=`expr $j + 1`
  fi
done

###	REGISTER CALL	###
  tes="$(grep 'call	r' $1)"
  _TRACE_ "REGISTER CALLS:" " $tes"
  ir=1
  countr=1
while [ $countr -le 1 ]
do
  SUBSTRINGR=$(echo $tes| cut -d';' -f $ir)
  #echo $SUBSTRINGR
  ir=`expr $ir + 1`
  
  if [ -z "$SUBSTRINGR" ]
  then
      countr=2
  else
      temprega=r$(echo $SUBSTRINGR| cut -d'r' -f 2)
      tempaddr=$(echo $SUBSTRINGR| cut -d':' -f 1)
      tempregb=",$temprega	;#0x"
      bigtes="$(grep "$tempregb" $1)"
      irr=1
      countrr=1
      while [ $countrr -le 1 ]
	do
	SUBSTRINGRR=$(echo $bigtes| cut -d';' -f $irr)
	irr=`expr $irr + 1`
	if [ -z "$SUBSTRINGRR" ]
	    then
	    countrr=2
	else 
	    regaddr=${SUBSTRINGRR:0:7}
	    finaladdr=$regaddr
	fi
      done 
      sed -i "s/$tempaddr:	call	$temprega/$tempaddr:	call	$temprega	;$finaladdr/g" $1
  fi
done
  
###	CALL	###  
  var="$(grep 'call	' $1)"
  _TRACE_ "CALLS:" "$var"
  
  i=2
  count=1
while [ $count -le 1 ]
do
  SUBSTRING=$(echo $var| cut -d'x' -f $i)
  # echo $SUBSTRING
  i=`expr $i + 1`
  if [ -z "$SUBSTRING" ]
  then
      count=2
  else
      addr=${SUBSTRING:0:4}
      tempiarray=0
      for j in "${iarray[@]}" 
	do
	if [ "$j" == "$addr" ]  
	then
	    tempiarray=1
	fi
      done
      if [ $tempiarray -ne 1 ]
	  then
	  tempiarray=0
	  iarray+=( "$addr")
	  vaddr="$(grep "$addr <" $1)"
	  vaddr=$(echo $vaddr| cut -d' ' -f 2)
	  vaddr=$(echo $vaddr| cut -d':' -f 1)
	  sed -i "s/;#0x$addr/;#0x$addr $vaddr/g" $1
      fi
  fi
  
done 

# --- added LBesnard 2019 December ----"
# -------------------------------------"

echo "   *** Removing unused entrypoints"

# ---- Cleaning code : unuseful label (cfg)
# The not declared labels in the SYMBOL TABLE are removed.
str="$(grep -e "<*>:" $1)"
_TRACE_  "set =" "${str}"
b="$(expr index "$str" '<')"
while [ "$b" != "0" ] 
do
  e="$(expr index "$str" '>')"
  v="$(echo ${str: $b : $e-$b-1})"
  _TRACE_ " label name = ${v}"
  dcl="$(grep -ce ' '${v}$ $1)"
  vall="$(echo ${str: 0: $e+1})"
  if [ "$dcl" == "0" ]
  then
    _TRACE_ " *** Removing the line with the label: ${v}"
    sed -i "/${vall}/d" $1
  fi

  str="$(echo ${str:${e}+1})"
  b="$(expr index "$str" '<')"
done

# Creating an array called TABCALLS that contains the calls (unicity)
# --------------------------------------------------------------------
calls="$(grep -e \<*\>$ $1)"
_TRACE_ " THE CALLS :" "$calls"

b="$(expr index "$calls" ';')"
while [ "$b" != "0" ] 
do
  e="$(expr index "$calls" '>')"
  vcall="$(echo ${calls:${b} : $e - $b})"
  insertCall "$vcall" 
  calls="$(echo ${calls:${e}+1})"
  b="$(expr index "$calls" ';')"
done

if [ "$b_TRACE_" == "true" ]; then printCalls; fi

# Creating an array called TABGOTO that contains the adress of the goto (br command)
# ---------------------------------------------------------------------------------
calls="$(grep -e br[[:space:]] $1)"

calls="$(echo ${calls//\t/ /})"
_TRACE_  " THE BREAKS:" "${calls}"

b="$(expr index "$calls" 'br')"
while [ "$b" != "0" ] 
do
  e="$(expr index "$calls" ';')"
  vcall="$(echo ${calls:${b}+1 : $e - $b -2})"
  insertGoto "${vcall}"
  calls="$(echo ${calls:${e}+1})"
  b="$(expr index "$calls" 'br')"
done

if [ "$b_TRACE_" == "true" ]; then printGotos; fi 

labels="$(grep -e "<*>:" $1)"
_TRACE_  " Labels =" "${labels}"

b="$(expr index "$labels" '>:')"
while [ "$b" != "0" ] 
do
  s="$(echo ${labels: 0 : $b })"
  u="$(expr index "$s" '<')"
  vaddr="$(echo ${s: 0: $u-2})"
  vaddr="$(echo "${vaddr#0000}")"
  vaddr="$(echo "${vaddr#000}")"
  vaddr="$(echo "${vaddr#00}")"
  TABFUNCTIONS[${#TABFUNCTIONS[*]}]=${vaddr}
  labels="$(echo ${labels:${b}+1})"
  b="$(expr index "$labels" '>:')"
done

if [ "$b_TRACE_" == "true" ]; then printFunctions; fi

# echo "   *** Removing unused entrypoints (2)"
for var in "${!TABFUNCTIONS[@]}" ; do      
    curElem="$(echo ${TABFUNCTIONS[$var]})"
    NextElem="$(echo ${TABFUNCTIONS[$var + 1]})"
    isInTABCALLS "$curElem"
    if [ $? == 0 ]
    then
      isInTABGOTOS "$curElem"
      if [ $? == 0 ]
      then
	# echo "      $curElem not in TABCALLS and not in TABGOTOS"
	# filtrer le main (mais il faudrait etre plus general )
	vaddr="$(echo ${curElem}:)"
	linevaddr="$(grep -n ${vaddr} $1)"
	u="$(expr index "$linevaddr" ':')"
	s="$(echo ${linevaddr: 0 : $u-1 })"
	p=$(($s - 1))
	entryPoint="$(sed -n "${p} p" $1)"
	# echo " ENTRY POINT = $curElem, next =  $NextElem "
	yy="$(echo "$entryPoint" | grep -c -e \<main\>)"
	if [ $yy == 0 ]
	then
	    revoveEntryPoint $curElem $NextElem
	fi
      fi
    fi
done 

echo "End script on objdump"
