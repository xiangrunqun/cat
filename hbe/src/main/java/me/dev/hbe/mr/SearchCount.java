package me.dev.hbe.mr;

import me.dev.util.LogUtil;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.URLDecoder;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class SearchCount {


    public static class TokenizerMapper
            extends Mapper<Object, Text, Text, IntWritable>{

        private final static IntWritable one = new IntWritable(1);
        private Text word = new Text();

        public void map(Object key, Text value, Mapper.Context context
        ) throws IOException, InterruptedException {
            String search_line = value.toString();
            // 读取文本的没一行
            LogUtil.info("mapper search_line:"+search_line);
            // 提取搜索关键字
            String search_text = getSearchWord(search_line);
            LogUtil.info("mapper search_text:"+search_text);
            word.set(search_text);
            context.write(word, one);
        }
    }


    public static class IntSumReducer
            extends Reducer<Text,IntWritable,Text,IntWritable> {
        private IntWritable result = new IntWritable();

        public void reduce(Text key, Iterable<IntWritable> values,
                           Reducer.Context context
        ) throws IOException, InterruptedException {
            int sum = 0;
            for (IntWritable val : values) {
                sum += val.get();
            }
            result.set(sum);
            context.write(key, result);

            String search_text = key.toString();
            LogUtil.info("reducer search_text:"+search_text);
            // 分解 search_text
            for(int i=0,j=search_text.length();i<j;i++){

            }
        }
    }

    public static String getSearchWord(String line)  {
        String reg = "text=(.*?) "
            , ret = null;
        ;

        Pattern pattern = Pattern.compile(reg);
        Matcher matcher = pattern.matcher(line);
        boolean find = matcher.find();
        if(find){
            ret = matcher.group(1);
            try {
                ret = URLDecoder.decode(ret,"utf-8");
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }
        }
        return ret;
    }


    public static void main(String[] args) throws Exception {
        String s = "192.168.1.108 - - [11/Mar/2017:05:23:24 -0500] \"GET /manager/html?text=hello HTTP/1.1\" 401 2474"
                ;




    }



}
