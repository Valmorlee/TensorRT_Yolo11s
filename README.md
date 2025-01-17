# 基于 TensorRT & ByteTrack 的 C++ 部署与训练 YOLO11s 图像推理与实时图传

## 写在前面
- **个人 IDE 选择** `JetBrains CLion` 
- **原作者仓库** ` https://github.com/emptysoal/TensorRT-YOLO11`
> 此仓库仅供自己 ***学习探究*** 与 ***深挖拓展*** 使用


## 一. 项目简介 

[//]: # ()
[//]: # (![LoveUBabe]&#40;/example_md/result.gif "LoveUBabe"&#41;)

- 基于 `TensorRT-v8` ，部署`YOLO11s` 实时跟踪图传；
- 模型转换方式：`.pt` -> `.onnx` -> `.plan(.engine)`；

## 二. 系统与环境配置

### 个人系统配置：
> `Intel i9-13900H` `GeForce RTX 4060 Laptop` `Ubuntu 24.04`

### 个人环境配置：

- `TensorRT 8.6.1.6`
- `OpenCV 4.10.0`
- `cuDNN 8.9.7`
- `CUDA 12.6`
- `Cmake 3.26`
    

## 三. 推理速度

> 基于 `Linux x86_64` 环境下的 `Ubuntu 24.04LTS` 系统得出 

|       |   图像推理   |  姿态监测   |  实时图传   |       
|:-----:|:--------:|:-------:|:-------:|
| `C++` | 300+ fps | 249 fps | 216 fps |       
