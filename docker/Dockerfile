# Commencing
FROM ubuntu:18.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update
RUN apt-get install -y vim=2:8.0.1453-1ubuntu1
RUN apt-get install -y openssl
RUN apt-get install -y libssl-dev
RUN apt-get install -y libcurl4-openssl-dev
RUN apt-get install -y git gpg
RUN apt-get install -y libomp-dev
RUN apt-get install -y gsl-bin libgslcblas0 libgsl23 libgsl-dev libxml2-dev
RUN apt-get install -y python3-venv python3-pip python3-dev build-essential cmake
RUN pip3 install --upgrade pip


### Install R

#httpv:/ cran.rs.udio.com/bin/linux/ubuntu/
#RUN sh -c 'echo "deb http://cran.rstudio.com/bin/linux/ubuntu/bionic-cran35/" >> /etc/apt/sources.list'
RUN sh -c "echo \"deb http://cran.rstudio.com/bin/linux/ubuntu bionic-cran35/\" >> /etc/apt/sources.list"
RUN apt-key adv --keyserver keyserver.ubuntu.com --recv-keys E084DAB9
RUN apt-get -y update
RUN apt-get -y install r-base
RUN apt-get -y install r-base-dev
RUN apt-get -y install r-recommended







#### Py-RerF
RUN pip3 install -e "git+https://github.com/neurodata/RerF.git@staging#egg=rerf&subdirectory=Python"



#### R-RerF

## install cran version first
RUN Rscript -e "install.packages('rerf', repos = 'https://cloud.r-project.org', dependencies = TRUE)"
RUN Rscript -e "install.packages('styler', repos = 'https://cloud.r-project.org')"

RUN mkdir /home/Downloads
RUN mkdir /home/.ssh

WORKDIR /home/Downloads

### Install dev-version
RUN git clone https://github.com/neurodata/RerF.git
## defaults to the staging branch
RUN cd RerF; Rscript -e "install.packages('R-Project/', type = 'source', repos = NULL)"

WORKDIR /home/

CMD ["/bin/bash"]
