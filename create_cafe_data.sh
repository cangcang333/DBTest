#!/bin/bash

rm -f cafe.data

trans_date=`date "+%Y%m%d"`
trans_time=`date "+%H%M%S"`
ref_nbr=1234567890
customer_id=333555777999

for i in `seq 0 200000`;
do
	line=`echo $trans_date,$trans_time,$ref_nbr,$customer_id`;
	echo $line >> cafe.data;
	trans_time=$(($trans_time+1));
	ref_nbr=$(($ref_nbr+1));
	customer_id=$(($customer_id+1));
done
