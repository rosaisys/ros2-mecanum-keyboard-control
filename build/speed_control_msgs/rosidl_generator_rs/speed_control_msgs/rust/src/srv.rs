#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};




// Corresponds to speed_control_msgs__srv__AdjustSpeed_Request

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct AdjustSpeed_Request {
    /// 方向：1=增加，-1=减少，0=重置
    pub direction: i8,

    /// 调整步长（默认0.1）
    pub step: f64,

}



impl Default for AdjustSpeed_Request {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::AdjustSpeed_Request::default())
  }
}

impl rosidl_runtime_rs::Message for AdjustSpeed_Request {
  type RmwMsg = super::srv::rmw::AdjustSpeed_Request;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        direction: msg.direction,
        step: msg.step,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      direction: msg.direction,
      step: msg.step,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      direction: msg.direction,
      step: msg.step,
    }
  }
}


// Corresponds to speed_control_msgs__srv__AdjustSpeed_Response

// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct AdjustSpeed_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub success: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub message: std::string::String,


    // This member is not documented.
    #[allow(missing_docs)]
    pub current_linear: f64,


    // This member is not documented.
    #[allow(missing_docs)]
    pub current_angular: f64,

}



impl Default for AdjustSpeed_Response {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::srv::rmw::AdjustSpeed_Response::default())
  }
}

impl rosidl_runtime_rs::Message for AdjustSpeed_Response {
  type RmwMsg = super::srv::rmw::AdjustSpeed_Response;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        success: msg.success,
        message: msg.message.as_str().into(),
        current_linear: msg.current_linear,
        current_angular: msg.current_angular,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      success: msg.success,
        message: msg.message.as_str().into(),
      current_linear: msg.current_linear,
      current_angular: msg.current_angular,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      success: msg.success,
      message: msg.message.to_string(),
      current_linear: msg.current_linear,
      current_angular: msg.current_angular,
    }
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


