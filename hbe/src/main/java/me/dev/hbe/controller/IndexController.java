package me.dev.hbe.controller;

import me.dev.util.LogUtil;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

/**
 * Created by Administrator on 2017/1/26 0026.
 */
@Controller
public class IndexController {

    private String fs_default_name = "hdfs://192.168.1.11:9000";

    @RequestMapping("upload_file") @ResponseBody
    public Map<String,Object> uploadFileToHdfs(@RequestParam Map<String,Object> map) throws Exception{
        // 上传文件到hdfs系统。。
        // hadoop课件的 云存储系统4 的6：54位置
        FileSystem hdfs = getFS();
        Path src = new Path("F:\\hdfs_test.log")
                , dest = new Path("/data/hdfs_test.log")
                ;
        LogUtil.info("----start copy file ---");
        hdfs.copyFromLocalFile(src, dest);
        LogUtil.info("----end copy file ---");
        map.put("ret","success");
        return map;
    }

    @RequestMapping("get_file") @ResponseBody
    public Map<String,Object> get_file(@RequestParam Map<String,Object> map) throws Exception{
        // 上传文件到hdfs系统。。
        // hadoop课件的 云存储系统4 的19：13位置
        FileSystem hdfs = getFS();
        Path dest = new Path("D:\\hdfs_test.log")
                , src = new Path("/data/hdfs_test.log")
                ;
        LogUtil.info("----start get file ---");
        hdfs.copyToLocalFile(src, dest );
        LogUtil.info("----end get file ---");
        map.put("ret","success");
        return map;
    }

    @RequestMapping("delete_file") @ResponseBody
    public Map<String,Object> delete_file(@RequestParam Map<String,Object> map) throws Exception{
        // 上传文件到hdfs系统。。
        // hadoop课件的 云存储系统4 的14：39位置
        FileSystem hdfs = getFS();
        Path dest = new Path("/data/hdfs_test.log")
                ;
        LogUtil.info("----start delete file ---");
        hdfs.delete(dest, false );
        LogUtil.info("----end delete file ---");
        map.put("ret","success");
        return map;
    }

    @RequestMapping("log") @ResponseBody
    public void log(@RequestParam Map<String,Object> map) throws Exception{
        LogUtil.logger.debug("---- this is debug msg.. ------");
        LogUtil.logger.info("---- this is info msg.. ------");
        LogUtil.logger.warn("---- this is warn msg.. ------");
        LogUtil.logger.error("---- this is error msg.. ------");
    }

    private FileSystem getFS() {
        Configuration conf = new Configuration();
        conf.set("fs.default.name",fs_default_name);
        FileSystem hdfs = null;
        String HADOOP_HOME = "E:\\big_data\\hadoop-3.0.0-alpha1";
        try {
            System.setProperty("hadoop.home.dir",HADOOP_HOME);
            System.setProperty("HADOOP_HOME", HADOOP_HOME);
            hdfs = FileSystem.get(conf);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return hdfs;
    }


    public static void main(String[] args) throws Exception {
        IndexController cc = new IndexController();
        Map<String,Object> map = new HashMap<>();
        cc.uploadFileToHdfs(map);
    }



}
