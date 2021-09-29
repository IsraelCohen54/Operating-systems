#!/bin/bash
# Israel Cohen

# The prog run assuming we have files hostnamectl, os-release in same dir.
# and same flag doesn't printed again


#if num of params is less than 2:
if [ "$#" -lt 1 ];
  then
  echo "Invalid input"
  exit 1
fi

#if first param isnt host or system its wrong as well:
if [ "$1" != "system" ]&&[ "$1" != "host" ];
  then
  echo "Invalid input"
  exit 1
fi



#boolean to choose flags
currently_sys=false 
currently_host=false

#boolean so that each flag called only once!
was_called_1=false
was_called_2=false
was_called_3=false
was_called_4=false
was_called_5=false
was_called_6=false
was_called_7=false
was_called_8=false
was_called_9=false
was_called_10=false
was_called_11=false
was_called_12=false


ok_flag=false #check if there is any propriate flag, else print all data of chosen file

for arg in "$@"
do
  #echo $arg
    case $arg in
        system)
        #updatee system only if host wasnt called
        if [ "$currently_host" == "false" ] ; then
            currently_sys=true
            #echo "23" $currently_sys
        fi
        #echo "check cur_sys" $currently_sys 
        ;;
        host)
        #updatee system only if host wasnt called
        if [ "$currently_sys" == "false" ] ; then
            currently_host=true
        fi
        #echo "check cur_host" $currently_host
        ;;
        --name)       
            if [[ $currently_host = "false" ]] && [[ $was_called_1 == "false" ]]; then              
            #logic: grep start with NAME, at file... than separate at =, take sec, remove ""
              grep '^NAME' ./os-release | awk -F"=" '{print $2}'| sed 's/\"//g'
              ok_flag=true
              was_called_1=true
            fi
        ;;
        --version)
            if [[ "$currently_host" == "false" ]] && [[ "$was_called_2" = "false" ]] ; then
              grep '^VERSION=' ./os-release | awk -F"=" '{print $2}'| sed 's/\"//g'
              ok_flag=true
              was_called_2=true
            fi
        ;;
        --pretty_name)
            if [[ "$currently_host" == "false" ]] && [[ "$was_called_3" = "false" ]] ; then
              grep '^PRETTY_NAME' ./os-release | awk -F"=" '{print $2}'| sed 's/\"//g'
              ok_flag=true
              was_called_3=true
            fi
        ;;
        --home_url)
            if [[ "$currently_host" == "false" ]] && [[ "$was_called_4" = "false" ]] ; then
              grep '^HOME_URL' ./os-release | awk -F"=" '{print $2}'| sed 's/\"//g'
              ok_flag=true
              was_called_4=true
            fi
        ;;
        --support_url)
            if [[ "$currently_host" == "false" ]] && [[ "$was_called_5" = "false" ]] ; then
              grep '^SUPPORT_URL' ./os-release | awk -F"=" '{print $2}'| sed 's/\"//g'
              ok_flag=true
              was_called_5=true
            fi
        ;;
        #now work only if host was arg 1:
        --static_hostname)
            if [[ "$currently_sys" == "false" ]] && [[ "$was_called_6" = "false" ]] ; then
              grep 'Static hostname' ./hostnamectl | awk -F": " '{print $2}'| sed 's/\"//g'
              ok_flag=true
              was_called_6=true
            fi
        ;;
        --icon_name)
            if [[ "$currently_sys" == "false"  ]] && [[ "$was_called_7" = "false" ]] ; then
              grep 'Icon name' ./hostnamectl | awk -F": " '{print $2}'| sed 's/\"//g'
              ok_flag=true
              was_called_7=true
            fi
        ;;
        --machine_id)
            if [[ "$currently_sys" == "false" ]] && [[ "$was_called_8" = "false" ]] ; then
              grep 'Machine ID' ./hostnamectl | awk -F": " '{print $2}'| sed 's/\"//g'
              ok_flag=true
              was_called_8=true
            fi
        ;;
        --boot_id)
            if [[ "$currently_sys" == "false" ]] && [[ "$was_called_9" = "false" ]] ; then
              grep 'Boot ID' ./hostnamectl | awk -F": " '{print $2}'| sed 's/\"//g'
              ok_flag=true
              was_called_9=true
            fi
        ;;
        --virtualization)
            if [[ "$currently_sys" == "false" ]] && [[ "$was_called_10" = "false" ]] ; then
              grep 'Virtualization' ./hostnamectl | awk -F": " '{print $2}'| sed 's/\"//g'
              ok_flag=true
              was_called_10=true
            fi
        ;;
        --kernel)
            if [[ "$currently_sys" == "false" ]] && [[ "$was_called_11" = "false" ]] ; then
              grep 'Kernel' ./hostnamectl | awk -F": " '{print $2}'| sed 's/\"//g'
              ok_flag=true
              was_called_11=true
            fi
        ;;
        --architecture)
            if [[ "$currently_sys" == "false" ]] && [[ "$was_called_12" = "false" ]] ; then
              grep 'Architecture' ./hostnamectl | awk -F": " '{print $2}'| sed 's/\"//g'
              ok_flag=true
              was_called_12=true
            fi
        ;;
        *)
        ;;
    esac
done

#print all file data if no flag detected

if [[ "$currently_sys" = true ]] && [[ "$ok_flag" = "false" ]]; then
    cat os-release
fi

if [[ "$currently_host" = true ]] && [[ "$ok_flag" = "false" ]]; then
    cat hostnamectl
fi

#zeroing to false at end, just in case...
currently_sys=false 
currently_host=false
