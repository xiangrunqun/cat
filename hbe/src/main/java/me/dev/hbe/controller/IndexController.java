package me.dev.hbe.controller;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;

import java.util.HashMap;
import java.util.Map;

/**
 * Created by Administrator on 2017/1/26 0026.
 */
@Controller
public class IndexController {

    @RequestMapping("upload_file") @ResponseBody
    public Map<String,Object> uploadFileToHdfs(@RequestParam Map<String,Object> map) throws Exception{
        // 上传文件到hdfs系统。。
        // hadoop课件的 云存储系统4 的6：54位置
        Configuration conf = new Configuration();
        conf.set("fs.default.name","hdfs://192.168.1.11:9000");
        FileSystem hdfs = FileSystem.get(conf);
        Path source = new Path("F:\\hdfs_test.log")
                , dest = new Path("/data")
                ;
        hdfs.copyFromLocalFile(source, dest);
        map.put("ret","success");
        return map;
    }


    public static void main(String[] args) throws Exception {
        IndexController cc = new IndexController();
        Map<String,Object> map = new HashMap<>();
        cc.uploadFileToHdfs(map);
    }



}
