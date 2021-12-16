# for line in `cut -f 2 -d '"' *.json |grep '[^(data)(type)(meta)(ipv4)(action){}, ]'`
# do
# 	# echo "if(j[${line}].is_null()||!j[${line}].is_number_integer()) return false"
# 	echo `echo $line | grep "[^{}]"`
# 	# refined=`echo $line | grep "[^{},]"`
# 	# echo "if(j[${refined}].is_null()||!j[${refined}].is_number_integer()) return false"

# done

for line in `cat tmp`
do
	echo "if(j[\"${line}\"].is_null()||!j[\"${line}\"].is_number_integer()) return false;"
done
