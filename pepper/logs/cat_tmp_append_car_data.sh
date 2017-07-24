cd `dirname $0`

if [ -s tmp_append_car_data.log ]
then
    cat tmp_append_car_data.log >> append_car_data.log
fi
rm tmp_append_car_data.log
