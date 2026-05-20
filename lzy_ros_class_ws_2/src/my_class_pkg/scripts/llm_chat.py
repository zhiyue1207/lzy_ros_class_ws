#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from openai import OpenAI
import rospy
from std_msgs.msg import String
import threading
import queue
import time

api_key = "sk-xO42RdWpyU8AZK8hsVmvQakCojuaofrlC8hsPwX93u7qJLYL"
base_url = "https://api.moonshot.cn/v1"

class LLM:
    def __init__(self):
        self.client = OpenAI(api_key=api_key, base_url=base_url)
        self.model = "moonshot-v1-8k"
        self.system_role_content = u"你是Kimi,由 Moonshot AI 提供的人工智能助手, 我们将会叫你的小名“小月”,你不会在你的回答中提及你的小名,你更擅长中文和英文的对话. 你会为用户提供安全,有帮助,准确的回答. 同时,你会拒绝一切涉及恐怖主义,种族歧视,黄色暴力等问题的回答"

        # 多轮对话记忆
        self.history = [{"role": "system", "content": self.system_role_content}]

        rospy.init_node('robot_voice_llm_node', anonymous=True)
        rospy.Subscriber("/speech/result", String, self.speech_result_callback)
        
        # 语音播报发布器
        self.talk_pub = rospy.Publisher("/talk", String, queue_size=5)
        
        # 添加处理队列和线程
        self.process_queue = queue.Queue()
        self.processing = False
        self.current_processing = None
        
        # 启动处理线程
        self.processor_thread = threading.Thread(target=self.process_queue_thread)
        self.processor_thread.daemon = True
        self.processor_thread.start()
        
        print("LLM节点已启动，等待语音输入...")

    def speech_result_callback(self, msg):
        """ROS回调函数，快速接收消息并放入队列"""
        result = msg.data
        if result and result.strip():  # 检查结果是否非空
            print(f"收到语音输入: [{result}]")
            
            # 将消息放入处理队列
            self.process_queue.put(result)
    
    def process_queue_thread(self):
        """处理队列的独立线程"""
        while not rospy.is_shutdown():
            try:
                # 从队列获取消息（非阻塞，避免线程卡死）
                try:
                    result = self.process_queue.get(timeout=0.1)
                except queue.Empty:
                    continue
                    
                # 如果有消息在处理，跳过新的消息
                if self.processing:
                    print("正在处理上一个请求，跳过新请求")
                    continue
                    
                # 开始处理
                self.processing = True
                self.current_processing = result
                
                try:
                    # 调用LLM处理
                    self.process_speech(result)
                except Exception as e:
                    print(f"处理语音时出错: {str(e)}")
                    import traceback
                    traceback.print_exc()
                finally:
                    # 处理完成
                    self.processing = False
                    self.current_processing = None
                    
            except Exception as e:
                print(f"处理线程出错: {str(e)}")
                time.sleep(0.1)
    
    def process_speech(self, result):
        """处理语音识别结果并调用LLM"""
        try:
            # 强制转 unicode，彻底解决中文编码报错
            if isinstance(result, bytes):
                result = result.decode('utf-8')
            
            # 添加到历史记录
            self.history.append({"role": "user", "content": result})
            
            print(f"正在调用LLM API处理: {result}")
            
            # 调用LLM API
            completion = self.client.chat.completions.create(
                model=self.model,
                messages=self.history,
                temperature=0.1
            )
            
            res = completion.choices[0].message.content
            self.history.append({"role": "assistant", "content": res})

            print("LLM 的返回结果: \n" + res)

            # 发布到语音播报
            self.talk_pub.publish(res)
            
        except Exception as e:
            print(f"调用LLM API时出错: {str(e)}")
            import traceback
            traceback.print_exc()
            
            # 发送错误提示
            error_msg = "抱歉，处理请求时出错了"
            self.talk_pub.publish(error_msg)

if __name__ == "__main__":
    try:
        llm = LLM()
        rospy.spin()
    except KeyboardInterrupt:
        print("退出")
    except Exception as e:
        print(f"程序异常: {str(e)}")
