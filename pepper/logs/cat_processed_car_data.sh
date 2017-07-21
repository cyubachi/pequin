cd `dirname $0`

if [ -s processed_car_data.log ]
then
    cat processed_car_data.log >> car_data.log
fi
rm processed_car_data.log
