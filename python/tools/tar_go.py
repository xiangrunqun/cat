# -*- coding: utf-8 -*-
import sys,os,shutil

''' only for windows '''

# current abs path..
# target_path = os.path.abspath('.')
target_path = ''

def get_context_name(line_arr):
    if('cbestask' in line_arr):return 'cbestask'
    elif ('cbesfile' in line_arr):return 'cbesfile'
    elif ('admin' in line_arr):return 'admin'
    elif ('basic' in line_arr):return 'basic'
    elif ('pss' in line_arr):return 'pss'
    elif ('sms' in line_arr):return 'sms'
    elif ('cbes' in line_arr):return 'cbes'
    else: return ''

def assemble_file(context_name, line_arr, dir_flag):
    # print('assemble_file:', context_name, line_arr, dir_flag)
    class_path_name = 'WEB-INF\\classes\\'
    dirs_tuple = os.path.split(line_arr[1])
    per_dir = dirs_tuple[0]
    # print('dirs_tuple:', dirs_tuple)
    # save file into output dir
    output_dir = target_path+'\\output\\'+context_name+'\\'
    if(dir_flag == 'source'):output_dir += class_path_name
    # elif('web_info' == dir_flag): do nothing
    elif('web_static' == dir_flag): per_dir.replace('webapp/','') # remove webapp string

    
    if(per_dir):output_dir += '\\'.join(per_dir.split('/'))
    # print('output_dir:',output_dir)
    if not os.path.exists(output_dir):os.makedirs(output_dir)

    file_name_ = dirs_tuple[1]

    src_path = output_dir.replace(target_path+'\\output',target_path)

    #file copy
    print('src_file:',src_path+file_name_,'desc_file:',output_dir+'\\'+file_name_)
    try:
        shutil.copyfile(src_path+'\\'+file_name_,output_dir+'\\'+file_name_)
        # copy inner class
        dir_path = output_dir.replace(target_path+'\\output',target_path)
        # print('list file dir :',dir_path)
        real_file_name_arr = file_name_.split('.class')
        # print('real_file_name_arr:',real_file_name_arr,file_name_)
        if(len(real_file_name_arr)>1):
            file_cls_arr = [file_cls for file_cls in os.listdir(dir_path) if os.path.isfile(dir_path+'\\'+file_cls) and '.class' in file_cls and real_file_name_arr[0]+'$' in file_cls]
            if(len(file_cls_arr)>0):
                print('list other files ', file_cls_arr)
                for ff in file_cls_arr:
                    shutil.copyfile(output_dir.replace(target_path+'\\output',target_path)+'\\'+ff,output_dir+'\\'+ff)

    except Exception as e:
        print('copy file error...', e)



def gogo():
    with open(target_path+'\\list', 'rb') as f:
        for line in f.readlines():
            line = line.strip()
            if( line ):
                line = line.decode('utf-8')
                if('main/java/' in line ):
                    # print(line.split('main/java/'))
                    line_arr = line.split('main/java/')
                    assemble_file(get_context_name(line_arr[0]),line_arr,'source')

                elif ('main/webapp/' in line ):
                    # print(line.split('main/webapp/'))
                    line_arr = line.split('main/webapp/')
                    dir_flag = 'web_static'
                    if('WEB-INF' in line_arr[0]):dir_flag = 'web_info'
                    assemble_file(get_context_name(line_arr[0]),line_arr, dir_flag)

                elif ('main/resources/' in line ):
                    # print(line.split('main/resources/'))
                    line_arr = line.split('main/resources/')
                    assemble_file(get_context_name(line_arr[0]),line_arr,'source')
            #if(line.index('main/java/') )


if __name__ == '__main__':
    # print(sys.argv[1])
    # needing augument to path of target...
    if(len(sys.argv)>1):
        target_path = sys.argv[1]
        if(target_path):gogo()
    else:
        print('no arugments using default')
        target_path = 'E:\\common_target'
        if(target_path):gogo()
    #                E:\common_target\admin\WEB-INF\classes\com\aipost\admin\controller\cust
    # ss = os.listdir('E:\\common_target\\admin\\WEB-INF\\classes\\com\\aipost\\admin\\controller\\cust')
    # print(ss)



