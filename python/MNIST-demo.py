# -*- coding: utf-8 -*-
"""
Created on Tue Jun 10 10:35:58 2025

@author: Slofty
"""

# Python
import torch
import torch.nn as nn
import torch.optim as optim
from torchvision import datasets, transforms

# Python
class SimpleMNISTNet(nn.Module):
    def __init__(self):
        super(SimpleMNISTNet, self).__init__()
        self.conv1 = nn.Conv2d(1, 16, 3, 1)
        self.relu = nn.ReLU()
        self.pool = nn.MaxPool2d(2)
        self.fc1 = nn.Linear(16 * 13 * 13, 10)
        self.softmax = nn.Softmax(dim=1)  # 新增softmax层

    def forward(self, x):
        x = self.pool(self.relu(self.conv1(x)))
        x = x.view(x.size(0), -1)
        x = self.fc1(x)
        x = self.softmax(x)  # 输出加softmax
        return x

# 2. 加载MNIST数据
transform = transforms.Compose([
    transforms.ToTensor(),
    transforms.Lambda(lambda x: (x > 0.5).float())  # 二值化处理
])
train_dataset = datasets.MNIST('./data', train=True, download=True, transform=transform)
train_loader = torch.utils.data.DataLoader(train_dataset, batch_size=64, shuffle=True)

# 3. 训练模型
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
model = SimpleMNISTNet().to(device)
optimizer = optim.Adam(model.parameters(), lr=0.001)
criterion = nn.CrossEntropyLoss()

model.train()
for epoch in range(10):
    for data, target in train_loader:
        data, target = data.to(device), target.to(device)
        optimizer.zero_grad()
        output = model(data)
        loss = criterion(output, target)
        loss.backward()
        optimizer.step()
    print(f"Epoch {epoch+1} finished.")

# 4. 导出为ONNX
model.eval()
dummy_input = torch.randn(1, 1, 28, 28, device=device)
torch.onnx.export(
    model,
    dummy_input,
    "simple_mnist_net.onnx",
    input_names=["input"],
    output_names=["output"],
    opset_version=11
)
print("模型已导出为 simple_mnist_net.onnx")



# 1. 加载测试集
transform = transforms.Compose([
    transforms.ToTensor(),
    transforms.Lambda(lambda x: (x > 0.5).float())  # 二值化处理
])
test_dataset = datasets.MNIST('./data', train=False, download=True, transform=transform)
test_loader = torch.utils.data.DataLoader(test_dataset, batch_size=1000, shuffle=False)

# 2. 评估模型
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
model.eval()
correct = 0
total = 0

with torch.no_grad():
    for data, target in test_loader:
        data, target = data.to(device), target.to(device)
        outputs = model(data)
        _, predicted = torch.max(outputs.data, 1)
        total += target.size(0)
        correct += (predicted == target).sum().item()

accuracy = 100 * correct / total
print(f"Test Accuracy: {accuracy:.2f}%")




import matplotlib.pyplot as plt

# 定义一个函数来显示 MNIST 样本
def plot_mnist_samples(dataset, num_samples=10):
    plt.figure(figsize=(10, 2))
    for i in range(num_samples):
        image, label = dataset[i]
        plt.subplot(1, num_samples, i + 1)
        plt.imshow(image.squeeze(), cmap='gray')  # 去掉通道维度
        plt.title(f"Label: {label}")
        plt.axis('off')
    plt.tight_layout()
    plt.show()

