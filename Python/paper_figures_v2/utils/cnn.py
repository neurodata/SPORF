import numpy as np
import time
from scipy import stats
import torch
import torch.nn as nn
import torch.utils.data as utils
from torch.utils.data import DataLoader
import torchvision.transforms as transforms

# Convolutional neural network (two convolutional layers)
class ImpulseModel(nn.Module):
    def __init__(self):
        super().__init__()
        self.layer1 = nn.Sequential(
            nn.Conv1d(1, 32, kernel_size=10, stride=1, padding=0),
            nn.ReLU(),
            nn.MaxPool1d(kernel_size=2, stride=2))
        self.layer2 = nn.Sequential(
            nn.Conv1d(32, 64, kernel_size=10, stride=1, padding=0),
            nn.ReLU(),
            nn.MaxPool1d(kernel_size=2, stride=2))
        self.drop_out = nn.Dropout(p=0.5)
        self.fc1 = nn.Linear(18*64, 500)
        self.fc2 = nn.Linear(500, 2)
        
    def forward(self, x):
        out = self.layer1(x)
        out = self.layer2(out)
        out = out.reshape(out.size(0), -1)
        out = self.drop_out(out)
        out = self.fc1(out)
        out = self.fc2(out)
        return out

class HVBarModel(nn.Module):
    def __init__(self):
        super().__init__()
        self.layer1 = nn.Sequential(
            nn.Conv2d(1, 32, kernel_size=5, stride=1, padding=0),
            nn.ReLU(),
            nn.MaxPool2d(kernel_size=2, stride=2))
        self.layer2 = nn.Sequential(
            nn.Conv2d(32, 64, kernel_size=5, stride=1, padding=0),
            nn.ReLU(),
            nn.MaxPool2d(kernel_size=2, stride=2))
        self.drop_out = nn.Dropout()
        self.fc1 = nn.Linear((4)**2 * 64, 200)
        self.fc2 = nn.Linear(200, 2)
        
    def forward(self, x):
        out = self.layer1(x)
        out = self.layer2(out)
        out = out.reshape(out.size(0), -1)
        out = self.drop_out(out)
        out = self.fc1(out)
        out = self.fc2(out)
        return out


class CircleModel(nn.Module):
    def __init__(self):
        super().__init__()
        self.layer1 = nn.Sequential(
            nn.Conv1d(1, 32, kernel_size=6, stride=1, padding=0),
            nn.ReLU(),
            nn.MaxPool1d(kernel_size=2, stride=2))
        self.layer2 = nn.Sequential(
            nn.Conv1d(32, 64, kernel_size=6, stride=1, padding=0),
            nn.ReLU(),
            nn.MaxPool1d(kernel_size=2, stride=2))
        self.drop_out = nn.Dropout()
        self.fc1 = nn.Linear(21 * 64, 200)
        self.fc2 = nn.Linear(200, 2)
        
    def forward(self, x):
        out = self.layer1(x)
        out = self.layer2(out)
        out = out.reshape(out.size(0), -1)
        out = self.drop_out(out)
        out = self.fc1(out)
        out = self.fc2(out)
        return out

# Convolutional neural network (two convolutional layers)
class MNISTModel(nn.Module):
    def __init__(self):
        super().__init__()
        self.layer1 = nn.Sequential(
            nn.Conv2d(1, 32, kernel_size=5, stride=1, padding=0),
            nn.ReLU(),
            nn.MaxPool2d(kernel_size=2, stride=2))
        self.layer2 = nn.Sequential(
            nn.Conv2d(32, 64, kernel_size=5, stride=1, padding=0),
            nn.ReLU(),
            nn.MaxPool2d(kernel_size=2, stride=2))
        self.drop_out = nn.Dropout()
        self.fc1 = nn.Linear((4)**2 * 64, 200)
        self.fc2 = nn.Linear(200, 10)
        
    def forward(self, x):
        out = self.layer1(x)
        out = self.layer2(out)
        out = out.reshape(out.size(0), -1)
        out = self.drop_out(out)
        out = self.fc1(out)
        out = self.fc2(out)
        return out

# Convolutional neural network (two convolutional layers)
class CifarModel(nn.Module):
    def __init__(self):
        super().__init__()
        self.layer1 = nn.Sequential(
            nn.Conv2d(3, 32, kernel_size=5, stride=1, padding=0),
            nn.ReLU(),
            nn.MaxPool2d(kernel_size=2, stride=2))
        self.layer2 = nn.Sequential(
            nn.Conv2d(32, 64, kernel_size=5, stride=1, padding=0),
            nn.ReLU(),
            nn.MaxPool2d(kernel_size=2, stride=2))
        self.drop_out = nn.Dropout()
        self.fc1 = nn.Linear((4)**2 * 100, 200)
        self.fc2 = nn.Linear(200, 10)
        
    def forward(self, x):
        out = self.layer1(x)
        out = self.layer2(out)
        out = out.reshape(out.size(0), -1)
        out = self.drop_out(out)
        out = self.fc1(out)
        out = self.drop_out(out)
        out = self.fc2(out)
        return out

class CNN():
    def __init__(
        self,
        model,
        shape,
        num_epochs = 100,
        batch_size = -1,
        learning_rate = 0.001,
        verbose = True,
        early_stopping = True
    ):
        """
        shape : channel x height x width if 2d
                channel x width if 1d
        """
        self.num_epochs = num_epochs
        self.batch_size = batch_size
        self.learning_rate = learning_rate
        self.verbose = verbose
        self.model = model()
        self.early_stopping = early_stopping
        self.shape = shape

    def fit(self,X,y):
        self.num_classes = len(np.unique(y))
        if self.batch_size == -1:
            batch_size = len(y)
        else:
            batch_size = self.batch_size

        tensor_x = torch.from_numpy(np.reshape(X,(X.shape[0],*self.shape))).float()
        tensor_y = torch.from_numpy(np.asarray(y)).long()

        train_tensor = utils.TensorDataset(tensor_x,tensor_y) # create your datset
        train_loader = DataLoader(dataset=train_tensor, batch_size=batch_size, shuffle=True)

        # Loss and optimizer
        criterion = nn.CrossEntropyLoss()
        optimizer = torch.optim.Adam(self.model.parameters(), lr=self.learning_rate)
        
        # Train the model        
        total_step = len(train_loader)
        loss_list = []
        acc_list = []

        prior_loss = []
        for epoch in range(self.num_epochs):
            for i, (images, labels) in enumerate(train_loader):
                #images = images.float(); labels = labels.long()
                # Run the forward pass
                outputs = self.model(images.float())
                loss = criterion(outputs, labels.long())
                loss_list.append(loss.item())

                # Backprop and perform Adam optimisation
                optimizer.zero_grad()
                loss.backward()
                optimizer.step()

                # Track the accuracy
                total = labels.size(0)
                _, predicted = torch.max(outputs.data, 1)
                correct = (predicted == labels).sum().item()
                acc_list.append(correct / total)
                curr_loss = loss.item()

                if ((i + 1) % 1 == 0) and ((epoch + 1) % 5 == 0) and self.verbose:
                    print('Epoch [{}/{}], Step [{}/{}], Loss: {:.4f}, Accuracy: {:.2f}%'
                        .format(epoch + 1, self.num_epochs, i + 1, total_step, curr_loss,
                                (correct / total) * 100))
            if epoch > 10 and self.early_stopping:
                if stats.sem(prior_loss[-5:]) < 0.001:
                    break
            prior_loss.append(loss.item())

        return(self)
        
    def predict(self,X):
        tensor_x = torch.from_numpy(np.reshape(X,(X.shape[0],*self.shape))).float()
        tensor_y = torch.from_numpy(np.zeros(X.shape[0])).long()

        test_tensor = utils.TensorDataset(tensor_x,tensor_y) # create your datset
        test_loader = DataLoader(dataset=test_tensor, batch_size=X.shape[0], shuffle=False)

        self.model.eval()

        with torch.no_grad():
            yhat = []
            for images, labels in test_loader:
                outputs = self.model(images)
                predicted = torch.max(outputs.data, 1)[1].data.numpy()
                yhat.append(predicted)
        
        return(np.concatenate(yhat))
            

    
    def score(self,X,y):
        return(np.mean(self.predict(X) == y))