.onLoad <- function(libname, pkgname) {
  token <- Sys.getenv("SLACK_API_TOKEN")
  if (token == "") {
    warning("SLACK_API_TOKEN environment variable not set")
  } else {
    set_slack_token(token)
  }
}