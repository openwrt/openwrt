module YAML
  class << self
    alias_method :load, :unsafe_load if YAML.respond_to? :unsafe_load
  end

  def self.LOG(info)
    puts Time.new.strftime("%Y-%m-%d %H:%M:%S") + " #{info}"
  end
end