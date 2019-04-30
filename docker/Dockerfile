# Commencing 
FROM ubuntu:18.04

#ENV DEBIAN_FRONTEND=noninteractive
#ENV TZ=US/Chicago
#RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

## Updates and software that I like
# Install vnc, xvfb in order to create a 'fake' display and firefox
RUN apt-get update
RUN apt-get install -y htop tmux
RUN apt-get install -y vim=2:8.0.1453-1ubuntu1
RUN apt-get install -y neovim
RUN apt-get update
RUN apt-get install -y ssh 
RUN apt-get install -y curl
RUN apt-get install -y python 
RUN apt-get install -y gpg 
RUN apt-get install -y dirmngr 
RUN apt-get update --fix-missing
RUN apt-get install -y openssl 
RUN apt-get install -y libssl-dev 
RUN apt-get install -y libcurl4-openssl-dev
RUN apt-get install -y pandoc
RUN apt-get install -y git
RUN apt-get install -y gsl-bin libgslcblas0 libgsl23 libgsl-dev libxml2-dev
RUN apt-get install -y python3-venv python3-dev
RUN apt-get install -y libomp-dev
RUN apt-get install -y man

RUN apt-get install -y python3-pip python3-dev build-essential cmake
RUN pip3 install --upgrade pip

RUN pip3 install -e "git+https://github.com/neurodata/RerF.git@staging#egg=rerf&subdirectory=Python"

CMD ["/bin/bash"]
