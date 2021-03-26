job "web" {
  datacenters = ["dc1"]
  type = "service"
  group "nginx" {
    task "nginx" {
      driver = "docker"
      config {
        image = "nginx"
        port_map {
          http = 80
        }
      }
      resources {
        network {
          port "http" {
            static = 80
          }
        }
      }
      service {
        name = "nginx"
        tags = ["dominican-front", "web", "nginx"]
        port = "http"
        check {
          type = "tcp"
          interval = "10s"
          timeout = "2s"
        }
      }
    }
  }
}
