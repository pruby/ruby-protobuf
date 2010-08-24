#include "ruby.h"

static VALUE Protobuf = Qnil;
static VALUE Field = Qnil;
static VALUE VarintField = Qnil;
static VALUE Fixnum = Qnil;

static ID IDrb_encode, IDrb_decode;

static VALUE varint_encode(VALUE self, VALUE rb_value) {
  char buf[32];
  size_t byte_count = 0;
  
  if (rb_obj_is_kind_of(rb_value, Fixnum)) {
    long value = NUM2LONG(rb_value);
    if (value < 0) rb_raise(rb_gv_get("RangeError"), "%d is negative", value);
    VALUE bytes = rb_str_new("", 0);
    while (value > 127) {
      char byte = (0x80 | (value));
      buf[byte_count++] = byte;
      value >>= 7;
      
      if (byte_count == 32) {
        rb_str_cat(bytes, buf, byte_count);
        byte_count = 0;
      }
    }
    buf[byte_count++] = value;
    rb_str_cat(bytes, buf, byte_count);
    
    return bytes;
  } else {
    /* Unsupported parameter type - pass through to Ruby version */
    return rb_funcall(self, IDrb_encode, rb_value);
  }
}

static VALUE varint_decode(VALUE self, VALUE value) {
  
}

void Init_protobuf_accel() {
  Protobuf = rb_define_module("Protobuf");
  Field = rb_const_get(Protobuf, rb_intern("Field"));
  VarintField = rb_const_get(Field, rb_intern("VarintField"));
  Fixnum = rb_const_get(rb_cObject, rb_intern("Fixnum"));
  
  IDrb_encode = rb_intern("rb_encode");
  IDrb_decode = rb_intern("rb_decode");
  
  rb_define_alias(VarintField, "rb_encode", "encode");
  rb_define_alias(VarintField, "rb_decode", "decode");
  
  rb_define_module_function(VarintField, "encode", varint_encode, 1);
  //rb_define_module_function(VarintField, "decode", varint_decode, 1);
}
