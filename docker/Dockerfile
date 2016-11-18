FROM ubuntu:16.04
RUN apt-get update
RUN apt-get install -y software-properties-common curl git openssh-client dictionaries-common wbritish
RUN add-apt-repository ppa:webupd8team/java
RUN apt-get update
RUN echo oracle-java8-installer shared/accepted-oracle-license-v1-1 select true | debconf-set-selections
RUN apt-get install -y oracle-java8-installer
RUN echo "deb [arch=amd64] http://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list
RUN curl https://bazel.build/bazel-release.pub.gpg | apt-key add -
RUN apt-get update && apt-get install -y bazel
ADD docker-bazel.id_rsa /root/.ssh/id_rsa
ADD github.rsa /root/.ssh/known_hosts
ADD bazelrc /root/bazelrc
RUN git clone git@github.com:hatstand/symmetrical-octo-fiesta.git
RUN cd symmetrical-octo-fiesta && bazel --bazelrc=/root/bazelrc build :server

EXPOSE 12345
WORKDIR /symmetrical-octo-fiesta
CMD ["/usr/bin/bazel", "run", ":server", "--", "--port", "12345"]
