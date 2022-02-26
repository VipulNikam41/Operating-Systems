#HW-Paging-LinearTranslate
cd HW-Paging-LinearTranslate/

# 1
python2 paging-linear-translate.py -P 1k -a 1m -p 512m -v -n 0
python2 paging-linear-translate.py -P 1k -a 2m -p 512m -v -n 0
python2 paging-linear-translate.py -P 1k -a 4m -p 512m -v -n 0

python2 paging-linear-translate.py -P 1k -a 1m -p 512m -v -n 0
python2 paging-linear-translate.py -P 2k -a 1m -p 512m -v -n 0
python2 paging-linear-translate.py -P 4k -a 1m -p 512m -v -n 0

# 2
python2 paging-linear-translate.py -P 1k -a 16k -p 32k -v -u 0
python2 paging-linear-translate.py -P 1k -a 16k -p 32k -v -u 25
python2 paging-linear-translate.py -P 1k -a 16k -p 32k -v -u 50
python2 paging-linear-translate.py -P 1k -a 16k -p 32k -v -u 75
python2 paging-linear-translate.py -P 1k -a 16k -p 32k -v -u 100

# 3
python2 paging-linear-translate.py -P 8 -a 32 -p 1024 -v -s 1
python2 paging-linear-translate.py -P 8k -a 32k -p 1m -v -s 2
python2 paging-linear-translate.py -P 1m -a 256m -p 512m -v -s 3


cd ..

# HW-Paging-MultiLevelTranslate
cd HW-Paging-MultiLevelTranslate/

# 2
python2 paging-multilevel-translate.py -s 0 -n 4 | grep "page 108"
