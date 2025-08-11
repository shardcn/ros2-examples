#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

// ROS2最小发布者节点实现
// 这个类继承自rclcpp::Node，实现了一个基本的消息发布功能
class MinimalPublisher : public rclcpp::Node
{
public:
  // 构造函数：初始化节点名称和计数器
  MinimalPublisher()
  : Node("minimal_publisher"), count_(0)  // 调用父类构造函数，设置节点名为"minimal_publisher"，初始化计数器为0
  {
    // 创建发布者对象
    // 参数说明：
    // - std_msgs::msg::String: 消息类型，发布字符串消息
    // - "topic": 话题名称，其他节点可以订阅这个话题来接收消息
    // - 10: 队列大小，表示最多缓存10条消息
    publisher_ = this->create_publisher<std_msgs::msg::String>("topic", 10);

    // 定义定时器回调函数（使用 Lambda 表达式）
    // 这个函数会被定时器周期性调用，用于发布消息
    auto timer_callback =
      [this]() -> void {
        // 创建一个字符串消息对象
        auto message = std_msgs::msg::String();

        // 设置消息内容，包含"Hello, world!"和递增的计数器
        // std::to_string() 将整数转换为字符串
        message.data = "Hello, world! " + std::to_string(this->count_++);

        // 打印日志信息到控制台
        // RCLCPP_INFO 是 ROS2 的日志宏，用于输出信息级别的日志
        // get_logger() 获取节点的日志记录器
        RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());

        // 通过发布者对象发布消息到话题
        this->publisher_->publish(message);
      };

    // 创建定时器
    // 参数说明：
    // - 500ms: 定时器周期，每 500 毫秒触发一次回调函数
    // - timer_callback: 定时器触发时执行的回调函数
    timer_ = this->create_wall_timer(500ms, timer_callback);
  }

private:
  // 私有成员变量
  rclcpp::TimerBase::SharedPtr timer_;           // 定时器智能指针，管理定时器对象的生命周期
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
  size_t count_;  // 消息计数器，记录已发送的消息数量
};

// 主函数：程序入口点
int main(int argc, char * argv[])
{
  // 初始化 ROS2 系统
  // 这是使用 ROS2 功能前必须调用的函数，处理命令行参数和系统初始化
  rclcpp::init(argc, argv);

  // 创建 MinimalPublisher 节点的智能指针并开始运行
  // rclcpp::spin() 会进入事件循环，处理定时器、回调函数等
  // 程序会持续运行直到接收到终止信号（如 Ctrl+C）
  rclcpp::spin(std::make_shared<MinimalPublisher>());

  // 清理 ROS2 系统资源
  // 在程序结束前调用，确保正确释放所有 ROS2 相关资源
  rclcpp::shutdown();

  // 程序正常结束
  return 0;
}