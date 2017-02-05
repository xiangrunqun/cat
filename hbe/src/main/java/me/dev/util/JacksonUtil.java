package me.dev.util;

import com.fasterxml.jackson.databind.ObjectMapper;
import org.springframework.util.StringUtils;

import java.io.IOException;
import java.util.Map;

/**
 * Created by Administrator on 2017/1/26 0026.
 */
public class JacksonUtil {

    public static final ObjectMapper mapper = new ObjectMapper();


    public static Map<String,Object> getMap(String content){
        Map<String,Object> map = null;
        if(StringUtils.isEmpty(content)) return null;
        try {
            map = mapper.readValue(content,Map.class);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return map;
    }

    public static String getJSON(Object obj){
        String json = null;
        if(obj == null) return null;
        try {
            json = mapper.writeValueAsString(obj);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return json;
    }


}
