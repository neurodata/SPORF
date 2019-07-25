# Commencing
FROM ubuntu:18.04


# System Setup
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
	&& apt-get install -y vim \
		openssl \
		libssl-dev \
		libcurl4-openssl-dev \
		git \
		gpg \
		libomp-dev \
		libeigen3-dev \
		gsl-bin \
		libgslcblas0 \
		libgsl23 \
		libgsl-dev \
		libxml2-dev


# Install python3
RUN apt-get install -y python3-venv python3-pip python3-dev build-essential cmake
RUN python3 -m pip install --upgrade pip setuptools wheel
RUN pip3 install pandas


# Install R
#httpv:/ cran.rs.udio.com/bin/linux/ubuntu/
#RUN sh -c 'echo "deb http://cran.rstudio.com/bin/linux/ubuntu/bionic-cran35/" >> /etc/apt/sources.list'
#RUN sh -c "echo \"deb http://cran.rstudio.com/bin/linux/ubuntu bionic-cran35/\" >> /etc/apt/sources.list"
#RUN apt-key adv --keyserver keyserver.ubuntu.com --recv-keys E084DAB9
#RUN apt-get -y update
#RUN apt-get -y install r-base
#RUN apt-get -y install r-base-dev
#RUN apt-get -y install r-recommended
#ENV DEBIAN_FRONTEND noninteractive


### Copied from https://github.com/rocker-org/rocker/blob/master/r-ubuntu/Dockerfile
# Now install R and littler, and create a link for littler in /usr/local/bin
# Default CRAN repo is now set by R itself, and littler knows about it too
# r-cran-docopt is not currently in c2d4u so we install from source
RUN apt-get update \
	    && apt-get install -y --no-install-recommends \
	             littler \
		 r-base \
		 r-base-dev \
		 r-recommended \
	&& ln -s /usr/lib/R/site-library/littler/examples/install.r /usr/local/bin/install.r \
	&& ln -s /usr/lib/R/site-library/littler/examples/install2.r /usr/local/bin/install2.r \
	&& ln -s /usr/lib/R/site-library/littler/examples/installGithub.r /usr/local/bin/installGithub.r \
	&& ln -s /usr/lib/R/site-library/littler/examples/testInstalled.r /usr/local/bin/testInstalled.r \
	&& install.r docopt \
	&& rm -rf /tmp/downloaded_packages/ /tmp/*.rds \
	&& rm -rf /var/lib/apt/lists/*
###




# Install RerF in various flavors

## Py-RerF
RUN pip3 install --no-binary rerf rerf



## R-RerF
### install cran version first for dependancies.
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
