#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};



#[link(name = "speed_control_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__speed_control_msgs__srv__AdjustSpeed_Request() -> *const std::ffi::c_void;
}

#[link(name = "speed_control_msgs__rosidl_generator_c")]
extern "C" {
    fn speed_control_msgs__srv__AdjustSpeed_Request__init(msg: *mut AdjustSpeed_Request) -> bool;
    fn speed_control_msgs__srv__AdjustSpeed_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<AdjustSpeed_Request>, size: usize) -> bool;
    fn speed_control_msgs__srv__AdjustSpeed_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<AdjustSpeed_Request>);
    fn speed_control_msgs__srv__AdjustSpeed_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<AdjustSpeed_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<AdjustSpeed_Request>) -> bool;
}

// Corresponds to speed_control_msgs__srv__AdjustSpeed_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct AdjustSpeed_Request {
    /// 方向：1=增加，-1=减少，0=重置
    pub direction: i8,

    /// 调整步长（默认0.1）
    pub step: f64,

}



impl Default for AdjustSpeed_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !speed_control_msgs__srv__AdjustSpeed_Request__init(&mut msg as *mut _) {
        panic!("Call to speed_control_msgs__srv__AdjustSpeed_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for AdjustSpeed_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { speed_control_msgs__srv__AdjustSpeed_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { speed_control_msgs__srv__AdjustSpeed_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { speed_control_msgs__srv__AdjustSpeed_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for AdjustSpeed_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for AdjustSpeed_Request where Self: Sized {
  const TYPE_NAME: &'static str = "speed_control_msgs/srv/AdjustSpeed_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__speed_control_msgs__srv__AdjustSpeed_Request() }
  }
}


#[link(name = "speed_control_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__speed_control_msgs__srv__AdjustSpeed_Response() -> *const std::ffi::c_void;
}

#[link(name = "speed_control_msgs__rosidl_generator_c")]
extern "C" {
    fn speed_control_msgs__srv__AdjustSpeed_Response__init(msg: *mut AdjustSpeed_Response) -> bool;
    fn speed_control_msgs__srv__AdjustSpeed_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<AdjustSpeed_Response>, size: usize) -> bool;
    fn speed_control_msgs__srv__AdjustSpeed_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<AdjustSpeed_Response>);
    fn speed_control_msgs__srv__AdjustSpeed_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<AdjustSpeed_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<AdjustSpeed_Response>) -> bool;
}

// Corresponds to speed_control_msgs__srv__AdjustSpeed_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct AdjustSpeed_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub message: rosidl_runtime_rs::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub current_linear: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub current_angular: f64,

}



impl Default for AdjustSpeed_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !speed_control_msgs__srv__AdjustSpeed_Response__init(&mut msg as *mut _) {
        panic!("Call to speed_control_msgs__srv__AdjustSpeed_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for AdjustSpeed_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { speed_control_msgs__srv__AdjustSpeed_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { speed_control_msgs__srv__AdjustSpeed_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { speed_control_msgs__srv__AdjustSpeed_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for AdjustSpeed_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for AdjustSpeed_Response where Self: Sized {
  const TYPE_NAME: &'static str = "speed_control_msgs/srv/AdjustSpeed_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__speed_control_msgs__srv__AdjustSpeed_Response() }
  }
}






#[link(name = "speed_control_msgs__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__speed_control_msgs__srv__AdjustSpeed() -> *const std::ffi::c_void;
}

// Corresponds to speed_control_msgs__srv__AdjustSpeed
#[allow(missing_docs, non_camel_case_types)]
pub struct AdjustSpeed;

impl rosidl_runtime_rs::Service for AdjustSpeed {
    type Request = AdjustSpeed_Request;
    type Response = AdjustSpeed_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__speed_control_msgs__srv__AdjustSpeed() }
    }
}


