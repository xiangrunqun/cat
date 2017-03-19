package me.dev.util;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.util.StringUtils;

/**
 * Created by Administrator on 2017/1/26 0026.
 */
public class LogUtil {
    public final static Logger logger = LoggerFactory.getLogger(LogUtil.class);

    public static void error(Exception e)
    {
        logger.error("",e);
    }

    /**
     * log info msg...
     * @param parameters
     */
    public static void info(String... parameters){
        String flag = "",parameter = "",result = "";
        switch (parameters.length){
            case 1: result = parameters[0]; break;
            case 2: result = parameters[1];parameter = parameters[0]; break;
            case 3: result = parameters[2];parameter = parameters[1];flag = parameters[0]; break;
        }
        StringBuilder msg = new StringBuilder();
        if(!StringUtils.isEmpty(flag)) msg.append("\t").append(flag);
        if(!StringUtils.isEmpty(parameter)) msg.append("\t").append(parameter);
        if(!StringUtils.isEmpty(result)) msg.append("\t").append(result);
        if(msg.length() > 0) logger.info(msg.toString());
    }


    public static void main(String[] args) {
        logger.debug("Entering application.");

        logger.info("Exiting application.");

    }
}
