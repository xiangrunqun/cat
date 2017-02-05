package me.dev.filter;

import me.dev.util.LogUtil;

import javax.servlet.*;
import javax.servlet.http.HttpServletRequest;

/**
 * Created by Administrator on 2017/1/26 0026.
 */
public class GFilter  implements Filter {

    private String encoding;

    public void doFilter(ServletRequest servletRequest, ServletResponse servletResponse, FilterChain filterChain) {
        try{
            servletRequest.setCharacterEncoding(encoding);
            HttpServletRequest req = (HttpServletRequest) servletRequest;
            String[] url_arr = req.getRequestURL().toString().split("/hbe/");
//            if(url_arr.length > 1){
//                String[] action_arr = url_arr[1].split("/");
//                if(action_arr.length > 1){
//                    String token;
//                    switch (action_arr[1]){
//                        case "login": break;
//                        default:
//                    }
//                }
//            }
        }catch (Exception e){
            LogUtil.error(e);
        }
        try { filterChain.doFilter(servletRequest, servletResponse); } catch (Exception e) { LogUtil.error(e); }
    }

    public void init(FilterConfig filterConfig) {
        this.encoding = filterConfig.getInitParameter("encoding");
    }

    @Override
    public void destroy() { }


}
