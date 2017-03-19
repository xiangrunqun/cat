package me.dev.util;

import com.lambdaworks.redis.RedisClient;
import com.lambdaworks.redis.RedisConnectionPool;
import com.lambdaworks.redis.RedisURI;
import com.lambdaworks.redis.SetArgs;
import com.lambdaworks.redis.api.sync.RedisCommands;
import com.lambdaworks.redis.codec.ByteArrayCodec;
import org.apache.commons.lang.StringUtils;

import java.util.List;

/**
 * cache domain
 *
 * @author Administrator
 *
 */
public class CachedRedis {

	private String cached_host_name;
	private String redis_passwd;
	private int max_active;
	private RedisClient redis_client;
	private RedisConnectionPool<RedisCommands<String, String>> connect_pool;
	private RedisConnectionPool<RedisCommands<byte[], byte[]>> connect_pool_byte;

	public void setCached_host_name(String cached_host_name) {
		this.cached_host_name = cached_host_name;
	}

	public void setMax_active(int max_active) {
		this.max_active = max_active;
	}

	public String get_cached_keys(String key_filter){
		try{
			RedisCommands<String, String> redis_command = connect_pool.allocateConnection();
			List<String> list = redis_command.keys(key_filter);
			if(list != null && list.size() > 0) return JacksonUtil.getJSON(list);
			connect_pool.freeConnection(redis_command);
		}catch (Exception e){
			LogUtil.error(e);
		}
		return null;
	}

	public boolean cached_data(String key, String value, long ttl){
		try{
			RedisCommands<String, String> redis_command = connect_pool.allocateConnection();
			LogUtil.info("cached_data before:" + ttl, "key:" + key, "value:" + value);
			if(StringUtils.isEmpty(value)){
				long del_cached = redis_command.del(key);
				LogUtil.info("cached_data del:" + del_cached, "key:" + key, "value:" + value);
			} else {
				String set_ret = redis_command.set(key, value, SetArgs.Builder.ex(ttl));
				LogUtil.info("cached_data set:" + set_ret, "key:" + key, "value:" + value);
			}
			connect_pool.freeConnection(redis_command);
			return true;
		}catch (Exception e){
			LogUtil.error(e);
			return false;
		}
	}

	public String get_cached_data(String key){
		try{
			LogUtil.info("get_cached_data before:", key);
			RedisCommands<String, String> redis_command = connect_pool.allocateConnection();
			String value = redis_command.get(key);
			LogUtil.info("get_cached_data after:", value);
			connect_pool.freeConnection(redis_command);
			return value==null?null:new String(value);
		}catch (Exception e){
			LogUtil.error(e);
			return null;
		}
	}

	public boolean cached_byte_data(String key, byte[] value, long ttl){
		try{
			RedisCommands<byte[], byte[]> redis_command = connect_pool_byte.allocateConnection();
			LogUtil.info("cached_byte_data before:" + ttl, "key:" + key, "value:" + value);

			if(value == null){
				long del_cached = redis_command.del(key.getBytes());
			} else {
				String set_ret = redis_command.set(key.getBytes(), value, SetArgs.Builder.ex(ttl));
			}
			connect_pool_byte.freeConnection(redis_command);
			return true;
		}catch (Exception e){
			LogUtil.error(e);
			return false;
		}
	}

	public byte[] get_cached_byte_data(String key){
		try{
			LogUtil.info("get_cached_byte_data before:", key);
			RedisCommands<byte[], byte[]> redis_command = connect_pool_byte.allocateConnection();
			byte[] value = redis_command.get(key.getBytes());
			connect_pool_byte.freeConnection(redis_command);
			return value;
		}catch (Exception e){
			LogUtil.error(e);
			return null;
		}
	}

	public void close(){
		try{
			connect_pool.close();
			connect_pool_byte.close();
			redis_client.shutdown();
		}catch (Exception e){
			LogUtil.error(e);
		}
	}

	public void init(){
		try{
			RedisURI redisUri = RedisURI.Builder.redis(cached_host_name).withPassword(redis_passwd).withDatabase(0).build();
			redis_client = RedisClient.create(redisUri);
			connect_pool = redis_client.pool(5, max_active);
			connect_pool_byte = redis_client.pool(new ByteArrayCodec(), 5, max_active);
		}catch (Exception e){
			LogUtil.error(e);
		}
	}

	public void setRedis_passwd(String redis_passwd) {
		this.redis_passwd = redis_passwd;
	}
}
