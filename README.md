1.rk3568开发板自身支持ov5695 V1_0的使用，只需要将其插入对应的插板槽中（需要注意防呆箭头）。

2.插入后，输入指令进行设备节点检查，

```linux
grep "" /sys/class/video4linux/video*/name
```

rkisp_mainpath是指rockchip isp的一个输出节点，可输出全分辨率图像，一般用来拍照，抓取Raw图。

rkisp_selfpath 是指Rockchip ISP的一个输出节点，最高只能输出1080p分辨率，一般用做预览。

statistics 是指3A统计

input-params 是3A参数设置

如果是测试后置摄像头，需要将以下命令中的“video1”修改为“video6”即可。

3.摄像头识别检测

```
v4l2-ctl --list-devices
```

4.参看摄像头支持格式

```
v4l2-ctl --list-formats-ext -d /dev/video0
```

5.然后使用RK提供好的摄像头使用脚本来运行摄像头，脚本运行命令如下：

```
/usr/local/bin/test_camera-rkisp1.sh
```

