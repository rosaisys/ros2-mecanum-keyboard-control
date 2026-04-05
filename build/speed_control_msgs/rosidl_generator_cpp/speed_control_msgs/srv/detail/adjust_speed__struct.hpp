// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from speed_control_msgs:srv/AdjustSpeed.idl
// generated code does not contain a copyright notice

#ifndef SPEED_CONTROL_MSGS__SRV__DETAIL__ADJUST_SPEED__STRUCT_HPP_
#define SPEED_CONTROL_MSGS__SRV__DETAIL__ADJUST_SPEED__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


#ifndef _WIN32
# define DEPRECATED__speed_control_msgs__srv__AdjustSpeed_Request __attribute__((deprecated))
#else
# define DEPRECATED__speed_control_msgs__srv__AdjustSpeed_Request __declspec(deprecated)
#endif

namespace speed_control_msgs
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct AdjustSpeed_Request_
{
  using Type = AdjustSpeed_Request_<ContainerAllocator>;

  explicit AdjustSpeed_Request_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->direction = 0;
      this->step = 0.0;
    }
  }

  explicit AdjustSpeed_Request_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->direction = 0;
      this->step = 0.0;
    }
  }

  // field types and members
  using _direction_type =
    int8_t;
  _direction_type direction;
  using _step_type =
    double;
  _step_type step;

  // setters for named parameter idiom
  Type & set__direction(
    const int8_t & _arg)
  {
    this->direction = _arg;
    return *this;
  }
  Type & set__step(
    const double & _arg)
  {
    this->step = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    speed_control_msgs::srv::AdjustSpeed_Request_<ContainerAllocator> *;
  using ConstRawPtr =
    const speed_control_msgs::srv::AdjustSpeed_Request_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<speed_control_msgs::srv::AdjustSpeed_Request_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<speed_control_msgs::srv::AdjustSpeed_Request_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      speed_control_msgs::srv::AdjustSpeed_Request_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<speed_control_msgs::srv::AdjustSpeed_Request_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      speed_control_msgs::srv::AdjustSpeed_Request_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<speed_control_msgs::srv::AdjustSpeed_Request_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<speed_control_msgs::srv::AdjustSpeed_Request_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<speed_control_msgs::srv::AdjustSpeed_Request_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__speed_control_msgs__srv__AdjustSpeed_Request
    std::shared_ptr<speed_control_msgs::srv::AdjustSpeed_Request_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__speed_control_msgs__srv__AdjustSpeed_Request
    std::shared_ptr<speed_control_msgs::srv::AdjustSpeed_Request_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const AdjustSpeed_Request_ & other) const
  {
    if (this->direction != other.direction) {
      return false;
    }
    if (this->step != other.step) {
      return false;
    }
    return true;
  }
  bool operator!=(const AdjustSpeed_Request_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct AdjustSpeed_Request_

// alias to use template instance with default allocator
using AdjustSpeed_Request =
  speed_control_msgs::srv::AdjustSpeed_Request_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace speed_control_msgs


#ifndef _WIN32
# define DEPRECATED__speed_control_msgs__srv__AdjustSpeed_Response __attribute__((deprecated))
#else
# define DEPRECATED__speed_control_msgs__srv__AdjustSpeed_Response __declspec(deprecated)
#endif

namespace speed_control_msgs
{

namespace srv
{

// message struct
template<class ContainerAllocator>
struct AdjustSpeed_Response_
{
  using Type = AdjustSpeed_Response_<ContainerAllocator>;

  explicit AdjustSpeed_Response_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->success = false;
      this->message = "";
      this->current_linear = 0.0;
      this->current_angular = 0.0;
    }
  }

  explicit AdjustSpeed_Response_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : message(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->success = false;
      this->message = "";
      this->current_linear = 0.0;
      this->current_angular = 0.0;
    }
  }

  // field types and members
  using _success_type =
    bool;
  _success_type success;
  using _message_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _message_type message;
  using _current_linear_type =
    double;
  _current_linear_type current_linear;
  using _current_angular_type =
    double;
  _current_angular_type current_angular;

  // setters for named parameter idiom
  Type & set__success(
    const bool & _arg)
  {
    this->success = _arg;
    return *this;
  }
  Type & set__message(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->message = _arg;
    return *this;
  }
  Type & set__current_linear(
    const double & _arg)
  {
    this->current_linear = _arg;
    return *this;
  }
  Type & set__current_angular(
    const double & _arg)
  {
    this->current_angular = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    speed_control_msgs::srv::AdjustSpeed_Response_<ContainerAllocator> *;
  using ConstRawPtr =
    const speed_control_msgs::srv::AdjustSpeed_Response_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<speed_control_msgs::srv::AdjustSpeed_Response_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<speed_control_msgs::srv::AdjustSpeed_Response_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      speed_control_msgs::srv::AdjustSpeed_Response_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<speed_control_msgs::srv::AdjustSpeed_Response_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      speed_control_msgs::srv::AdjustSpeed_Response_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<speed_control_msgs::srv::AdjustSpeed_Response_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<speed_control_msgs::srv::AdjustSpeed_Response_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<speed_control_msgs::srv::AdjustSpeed_Response_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__speed_control_msgs__srv__AdjustSpeed_Response
    std::shared_ptr<speed_control_msgs::srv::AdjustSpeed_Response_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__speed_control_msgs__srv__AdjustSpeed_Response
    std::shared_ptr<speed_control_msgs::srv::AdjustSpeed_Response_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const AdjustSpeed_Response_ & other) const
  {
    if (this->success != other.success) {
      return false;
    }
    if (this->message != other.message) {
      return false;
    }
    if (this->current_linear != other.current_linear) {
      return false;
    }
    if (this->current_angular != other.current_angular) {
      return false;
    }
    return true;
  }
  bool operator!=(const AdjustSpeed_Response_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct AdjustSpeed_Response_

// alias to use template instance with default allocator
using AdjustSpeed_Response =
  speed_control_msgs::srv::AdjustSpeed_Response_<std::allocator<void>>;

// constant definitions

}  // namespace srv

}  // namespace speed_control_msgs

namespace speed_control_msgs
{

namespace srv
{

struct AdjustSpeed
{
  using Request = speed_control_msgs::srv::AdjustSpeed_Request;
  using Response = speed_control_msgs::srv::AdjustSpeed_Response;
};

}  // namespace srv

}  // namespace speed_control_msgs

#endif  // SPEED_CONTROL_MSGS__SRV__DETAIL__ADJUST_SPEED__STRUCT_HPP_
